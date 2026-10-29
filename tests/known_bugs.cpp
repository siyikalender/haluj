/// \file known_bugs.cpp
/// Regression checks for bugs that are known but not fixed yet.
///
/// Each case checks the CORRECT behaviour and is registered in CMake with
/// WILL_FAIL, so today it fails and ctest reports it as passing. When a bug
/// is fixed its case starts to pass, ctest reports it as failing, and the
/// case should move to the ordinary tests.
///
/// Bugs with undefined behaviour, an endless loop or a build error are not
/// run here, because they may pass silently, hang or break the build. They
/// are listed for tracking (headers that do not build on their own are
/// listed in tests/CMakeLists.txt):
///   - bounded::push_back writes past the end when the vector is full
///     (size() <= capacity() instead of <)
///   - format(float) writes past last when the buffer is too small
///   - ring_buffer::back() reads m_container[-1] when m_head == 0
///   - on_t keeps a reference to the temporary of its default argument
///   - zm/om loop forever on a rule that succeeds without consuming,
///     for example zm(opt(ch('x')))
///   - format(int) negates INT_MIN
///   - timer_implementations/chrono.hpp and proxy.hpp share an include guard
/// build mode, so the tests also run in release (NDEBUG) builds.
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
#include <cstdio>
#include <cstring>
#include <iterator>
#include <optional>
#include <string>
#include <tuple>
#include <utility>

#include "haluj/bidirectional_map.hpp"
#include "haluj/bounded/vector.hpp"
#include "haluj/cyclic_index.hpp"
#include "haluj/digital_input_filter.hpp"
#include "haluj/format.hpp"
#include "haluj/ring_buffer.hpp"
#include "haluj/state_machine.hpp"
#include "haluj/timer.hpp"
#include "haluj/timer_implementations/software.hpp"

#include "check.hpp"

namespace sw = haluj::timer_implementations::software;

// cyclic_increment(v, size, N) must wrap when v + size == N
static void cyclic_increment_n()
{
  HALUJ_CHECK(haluj::cyclic_increment(6u, 2u, 8u) == 0u);
}

// cyclic_distance from 2 to 5 in a ring of 8 is 3
static void cyclic_distance()
{
  HALUJ_CHECK(haluj::cyclic_distance(2u, 5u, 8u) == 3u);
  HALUJ_CHECK(haluj::cyclic_distance(5u, 2u, 8u) == 5u);
}

// Pushing into a full ring buffer must either be refused or drop the oldest
// element; today the newest element becomes the front
static void ring_buffer_push_full()
{
  haluj::ring_buffer<std::array<int, 4>> r;
  for (int i = 1; i <= 5; ++i)
    r.push(i);

  HALUJ_CHECK(r.front() == 1 || r.front() == 2);
}

// Assigning exactly capacity elements must succeed
static void vector_assign_full()
{
  const int source[] = {1, 2, 3, 4};
  haluj::bounded::vector<int, 4> v;
  HALUJ_CHECK(haluj::bounded::assign(v, std::begin(source), std::end(source)));
}

// A number that does not fit must not be written as a different number
static void format_int_truncation()
{
  char  text[8] = {};
  char* end     = haluj::format(12345, text, text + 3);
  HALUJ_CHECK(std::string(text, end) != "345");
}

// A backward timer with an unsigned duration must fire when a step jumps
// over zero
static void timer_backward_unsigned()
{
  haluj::timer<sw::bwd<unsigned>> t;
  t.set(5);

  int fired = 0;
  for (int i = 0; i < 10; ++i)
    fired += t(nullptr, 2u) ? 1 : 0;

  HALUJ_CHECK(fired > 0);
}

// A periodic forward timer must keep the overshoot: period 3, step 2,
// 24 time units -> 8 periods
static void timer_forward_drift()
{
  haluj::timer<sw::fwd<unsigned>> t;
  t.set(3);

  int fired = 0;
  for (int i = 0; i < 12; ++i)
    fired += t(nullptr, 2u) ? 1 : 0;

  HALUJ_CHECK(fired == 8);
}

// A single low sample must not release a debounced input
static void digital_filter_glitch()
{
  digital_input_filter<unsigned, 4> f(0x1u);
  for (int i = 0; i < 4; ++i)
    f(1u);

  HALUJ_CHECK(f(0u) == 1u);
}

// to_first over an array must use the given comparator
static void to_first_comparator()
{
  static const std::pair<int, const char*> table[] = {{1, "on"}, {2, "off"}};

  char        buffer[] = "off";
  const char* key      = buffer;

  auto code = haluj::to_first<std::optional>(key, table, haluj::c_str_equal_to());
  HALUJ_CHECK(code.has_value() && *code == 2);
}

// An rvalue argument must reach the transition action intact
static void state_machine_rvalue_argument()
{
  enum class st { idle, run };

  static std::string received;
  received.clear();

  auto m = haluj::machine(
    haluj::graph(
      haluj::transition<st::idle, st::run>(
        [](std::string e) { return e == "go"; },
        [](std::string e) { received = e; })),
    haluj::map(
      haluj::entry<st::idle>(std::make_tuple(nullptr, nullptr, nullptr))));

  m(st::idle, std::string("go"));
  HALUJ_CHECK(received == "go");
}

struct known_bug
{
  const char* name;
  void      (*run)();
};

static const known_bug bugs[] =
{
  {"cyclic_increment_n",            cyclic_increment_n},
  {"cyclic_distance",               cyclic_distance},
  {"ring_buffer_push_full",         ring_buffer_push_full},
  {"vector_assign_full",            vector_assign_full},
  {"format_int_truncation",         format_int_truncation},
  {"timer_backward_unsigned",       timer_backward_unsigned},
  {"timer_forward_drift",           timer_forward_drift},
  {"digital_filter_glitch",         digital_filter_glitch},
  {"to_first_comparator",           to_first_comparator},
  {"state_machine_rvalue_argument", state_machine_rvalue_argument},
};

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::fprintf(stderr, "usage: %s <case>\n", argv[0]);
    // Exit code 0, so that a case misspelled in CMake fails under WILL_FAIL
    return 0;
  }

  for (const known_bug& bug : bugs)
  {
    if (std::strcmp(bug.name, argv[1]) == 0)
    {
      bug.run();
      return haluj_test::report(bug.name);
    }
  }

  std::fprintf(stderr, "unknown case: %s\n", argv[1]);
  return 0;
}
