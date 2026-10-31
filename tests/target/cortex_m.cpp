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

#include "haluj/bidirectional_map.hpp"
#include "haluj/bounded/vector.hpp"
#include "haluj/digital_input_filter.hpp"
#include "haluj/format.hpp"
#include "haluj/parser.hpp"
#include "haluj/ring_buffer.hpp"
#include "haluj/set_and_wait.hpp"
#include "haluj/state_machine.hpp"
#include "haluj/timer.hpp"
#include "haluj/timer_implementations/software.hpp"

namespace
{

enum class mode { idle, run };

haluj::ring_buffer<std::array<char, 64>>                          rx;
haluj::bounded::vector<char, 32>                                  line;
haluj::timer<haluj::timer_implementations::software::fwd<std::uint32_t>> blink;
haluj::digital_input_filter<std::uint32_t, 4>                     buttons(0x3u);
haluj::set_and_wait                                               handshake;
mode                                                              state = mode::idle;
std::uint32_t                                                     setpoint = 0;

const std::pair<std::uint8_t, mode> modes[] = {{0, mode::idle}, {1, mode::run}};

} // namespace

// Called with each received byte
extern "C" void uart_rx(char c)
{
  if (!rx.full())
    rx.push(c);
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

  std::uint32_t led = 0;
  if (!blink.is_running())
    blink.set(500u);
  if (blink(nullptr, 1u))
    led = 1u;

  auto pressed = buttons(pins);

  auto target = haluj::to_second<std::optional>(requested, modes);

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
