/// \file test_constinit.cpp
/// Every type of the library can be built at compile time.
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
#include <chrono>
#include <cstdint>

#include "haluj/bounded/vector.hpp"
#include "haluj/cyclic_index.hpp"
#include "haluj/digital_input_filter.hpp"
#include "haluj/event_strategy.hpp"
#include "haluj/ring_buffer.hpp"
#include "haluj/set_and_wait.hpp"
#include "haluj/timer.hpp"
#include "haluj/timer_implementations/chrono.hpp"
#include "haluj/timer_implementations/software.hpp"
#include "haluj/utility.hpp"

#include "check.hpp"

// A global declared constinit must be initialized at compile time, or the
// program does not compile. On a microcontroller this means the object is in
// .bss or .data when the program starts: no constructor runs before main,
// and none can run after an interrupt has used the object.
namespace
{

enum class event { none, ready };

constinit haluj::ring_buffer<std::array<char, 8>>                      rb;
constinit haluj::ring_buffer<std::array<char, 8>, haluj::spsc>         rb_spsc;
constinit haluj::bounded::vector<int, 4>                               vec;
constinit haluj::cyclic_index<unsigned, 5>                             index;
constinit haluj::digital_input_filter<std::uint8_t, 3>                 filter(0x0Fu);
constinit haluj::strategy::blackboard<event>                           board;
constinit haluj::strategy::blackboard<event, haluj::spsc>              board_spsc;
constinit haluj::set_and_wait                                          handshake;
constinit haluj::async_loop<int>                                       loop(3);
constinit haluj::timer<haluj::timer_implementations::software::fwd<unsigned>> fwd;
constinit haluj::timer<haluj::timer_implementations::software::bwd<int>>      bwd;
constinit haluj::timer<haluj::timer_implementations::chrono<
  std::chrono::steady_clock, std::chrono::milliseconds>>               wall;

} // namespace

// The globals start in the state their constructors give
static void test_initial_state()
{
  HALUJ_CHECK(rb.empty() && rb_spsc.empty());
  HALUJ_CHECK(vec.empty());
  HALUJ_CHECK(*index == 0u);
  HALUJ_CHECK(filter.mask() == 0x0Fu && filter.value() == 0u);
  HALUJ_CHECK(!board.test(event::none) && !board_spsc.test(event::none));
  HALUJ_CHECK(loop.value_ == 3);
  HALUJ_CHECK(handshake.state_ == haluj::set_and_wait::states::set);
  HALUJ_CHECK(!fwd.is_running() && !bwd.is_running() && !wall.is_running());
}

// The single context types also work inside a constant expression
static_assert([]
{
  haluj::ring_buffer<std::array<int, 3>> r;
  r.push(1);
  r.push(2);
  int v = 0;
  r.try_pop(v);
  return v == 1 && r.size() == 1u;
}(), "ring_buffer at compile time");

static_assert([]
{
  haluj::bounded::vector<int, 4> v;
  v.push_back(7);
  v.resize(3, 1);
  return v.size() == 3u && v[0] == 7 && v[2] == 1;
}(), "bounded::vector at compile time");

static_assert([]
{
  haluj::digital_input_filter<unsigned, 2> f(1u);
  f(1u);
  return f(1u) == 1u && f.edge() == 1u;
}(), "digital_input_filter at compile time");

int main()
{
  test_initial_state();
  return haluj_test::report("test_constinit");
}
