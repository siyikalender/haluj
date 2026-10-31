/// \file cortex_m.cpp
/// Bare metal build check: a typical firmware use of haluj, compiled with
/// arm-none-eabi-g++ -fno-exceptions -fno-rtti. Nothing here may need a heap,
/// exceptions, RTTI or an operating system.
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

/// \author Claude Opus 5.5 <noreply@anthropic.com>
/// \date   2026

#include <array>
#include <cstdint>
#include <optional>
#include <utility>

#include "haluj/bounded/vector.hpp"
#include "haluj/digital_input_filter.hpp"
#include "haluj/event_strategy.hpp"
#include "haluj/format.hpp"
#include "haluj/pair_lookup.hpp"
#include "haluj/parser.hpp"
#include "haluj/ring_buffer.hpp"
#include "haluj/set_and_wait.hpp"
#include "haluj/state_machine.hpp"
#include "haluj/timer.hpp"
#include "haluj/timer_implementations/proxy.hpp"
#include "haluj/timer_implementations/software.hpp"

namespace
{

enum class mode { idle, run };
enum class dma_event : std::uint8_t { none, half, complete };

// constinit: every global is built at compile time and starts in .bss or
// .data, so nothing runs before main and an interrupt may come at any time
constinit haluj::ring_buffer<std::array<char, 64>, haluj::spsc>             rx;   // filled by the UART interrupt
constinit haluj::bounded::vector<char, 32>                                  line;
constinit haluj::timer<haluj::timer_implementations::software::fwd<std::uint32_t>> blink;
constinit haluj::digital_input_filter<std::uint32_t, 4>                     buttons(0x3u);
constinit haluj::set_and_wait                                               handshake;
constinit haluj::strategy::blackboard<dma_event, haluj::spsc>               dma;  // raised by the DMA interrupt
constinit mode                                                              state = mode::idle;
constinit std::uint32_t                                                     setpoint = 0;

const std::pair<std::uint8_t, mode> modes[] = {{0, mode::idle}, {1, mode::run}};

// A hardware timer made of static functions over a millisecond counter that
// an interrupt advances, as on a SysTick
struct systick_timer
{
  typedef std::uint32_t duration;

  static volatile std::uint32_t ticks;
  static std::uint32_t          timeout;
  static std::uint32_t          last;
  static bool                   running;

  static void load(duration p_value) { timeout = p_value; }
  static void start()                { running = true; }
  static void stop()                 { running = false; }
  static bool is_running()           { return running; }
  static void reset()                { last = ticks; }
  static void reload()               { last += timeout; }

  // The counter keeps the time, so the delta is not needed
  static bool proceed(duration)
  {
    return running && (std::uint32_t(ticks - last) >= timeout);
  }
};

volatile std::uint32_t systick_timer::ticks   = 0;
std::uint32_t          systick_timer::timeout = 0;
std::uint32_t          systick_timer::last    = 0;
bool                   systick_timer::running = false;

constinit haluj::timer<haluj::timer_implementations::proxy<systick_timer>> watchdog;

} // namespace

// Called from the SysTick interrupt
extern "C" void systick_handler()
{
  systick_timer::ticks = systick_timer::ticks + 1u;
}

// Called with each received byte
extern "C" void uart_rx(char c)
{
  if (!rx.full())
    rx.push(c);
}

// Called from the DMA interrupt; an event the main loop has not taken yet
// stays, the new one is dropped
extern "C" bool dma_isr(bool complete)
{
  return dma.raise(complete ? dma_event::complete : dma_event::half);
}

// Main loop side: takes the completion when it is there
extern "C" bool dma_done()
{
  return dma.test_and_clear(dma_event::complete);
}

// Parses "SET <n>" commands
extern "C" bool handle_line(const char* first, const char* last)
{
  using namespace haluj;

  auto number = action(om(digit()), [](const char* f, const char* l)
  {
    std::uint32_t v = 0;
    for (; f != l; ++f)
      v = v * 10u + static_cast<std::uint32_t>(*f - '0');
    setpoint = v;
  });

  return seq(ch('S'), ch('E'), ch('T'), om(space()), number).accept(first, last);
}

// A recursive grammar: arithmetic expressions with parentheses
struct expression : haluj::recursive<expression>
{
  static constexpr auto definition();
};

constexpr auto factor = haluj::any(haluj::om(haluj::digit()),
                                   haluj::seq(haluj::ch('('), expression{}, haluj::ch(')')));
constexpr auto term   = haluj::seq(factor,
                                   haluj::zm(haluj::seq(haluj::any(haluj::ch('*'), haluj::ch('/')), factor)));

constexpr auto expression::definition()
{
  return haluj::seq(term, haluj::zm(haluj::seq(haluj::any(haluj::ch('+'), haluj::ch('-')), term)));
}

extern "C" bool is_expression(const char* first, const char* last)
{
  return expression{}.accept(first, last) && first == last;
}

// Formats the set point for the display
extern "C" char* show_setpoint(char* first, char* last)
{
  return haluj::format(static_cast<int>(setpoint), first, last);
}

// Called every millisecond from the main loop
extern "C" std::uint32_t tick(std::uint32_t pins, std::uint8_t requested)
{
  while (!rx.empty())
  {
    char c = rx.front();
    rx.pop();
    if (c == '\n')
    {
      handle_line(line.data(), line.data() + line.size());
      line.clear();
    }
    else
    {
      haluj::bounded::push_back(line, c);
    }
  }

  if (!watchdog.is_running())
    watchdog.set(100u);
  if (watchdog(nullptr, 0u))
    handshake = haluj::set_and_wait();

  std::uint32_t led = 0;
  if (!blink.is_running())
    blink.set(500u);
  if (blink(nullptr, 1u))
    led = 1u;

  auto pressed = buttons(pins);

  auto target = haluj::to_second(requested, modes);

  auto m = haluj::machine(
    haluj::graph(
      haluj::transition<mode::idle, mode::run>(
        [](std::uint32_t p, bool) { return (p & 0x1u) != 0; }),
      haluj::transition<mode::run, mode::idle>(
        [](std::uint32_t p, bool) { return (p & 0x2u) != 0; }),
      haluj::transition<mode::idle, mode::run>(
        [](std::uint32_t, bool go) { return go; })),
    haluj::map(
      haluj::entry<mode::run>(std::make_tuple(
        [](std::uint32_t, bool) { setpoint = 0; }, nullptr, nullptr))));

  state = m(state, pressed, target.has_value() && *target == mode::run);

  handshake([] {}, [] { return true; });

  return led | (state == mode::run ? 2u : 0u);
}
