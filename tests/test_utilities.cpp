/// \file test_utilities.cpp
/// Tests of bidirectional_map, digital_input_filter, event_strategy,
/// set_and_wait and utility.
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

#include <cstring>
#include <iterator>
#include <optional>
#include <utility>

#include "haluj/bidirectional_map.hpp"
#include "haluj/digital_input_filter.hpp"
#include "haluj/event_strategy.hpp"
#include "haluj/set_and_wait.hpp"
#include "haluj/utility.hpp"

#include "check.hpp"

static void test_bidirectional_map()
{
  static const std::pair<int, const char*> table[] =
  {
    {1, "on"},
    {2, "off"}
  };

  auto name = haluj::to_second<std::optional>(2, table);
  HALUJ_CHECK(name.has_value());
  HALUJ_CHECK(std::strcmp(*name, "off") == 0);
  HALUJ_CHECK(!haluj::to_second<std::optional>(3, table).has_value());

  // A copy, so that pointer comparison alone would not find it
  char        buffer[] = "on";
  const char* key      = buffer;

  auto code = haluj::to_first<std::optional>(key,
                                             std::begin(table),
                                             std::end(table),
                                             haluj::c_str_equal_to());
  HALUJ_CHECK(code.has_value());
  HALUJ_CHECK(*code == 1);

  HALUJ_CHECK(haluj::find_in_first(1, std::begin(table), std::end(table)) == std::begin(table));
}

static void test_digital_input_filter()
{
  digital_input_filter<unsigned, 3> f(0x1u);

  HALUJ_CHECK(f(1u) == 0u);
  HALUJ_CHECK(f(1u) == 0u);
  HALUJ_CHECK(f(1u) == 1u); // three equal samples
  HALUJ_CHECK(f.edge() == 1u);
  HALUJ_CHECK(f(1u) == 1u);
  HALUJ_CHECK(f.edge() == 0u);

  // Bits outside the mask are ignored
  digital_input_filter<unsigned, 2> g(0x1u);
  g(0x2u);
  HALUJ_CHECK(g(0x2u) == 0u);
  HALUJ_CHECK(g.mask() == 0x1u);
}

static void test_blackboard()
{
  haluj::strategy::blackboard<int> board;

  HALUJ_CHECK(!board.test(3));
  HALUJ_CHECK(board.raise(3));
  HALUJ_CHECK(!board.raise(4)); // one pending event at a time
  HALUJ_CHECK(board.test(3));
  HALUJ_CHECK(!board.test(4));
  HALUJ_CHECK(haluj::on(3, board)());

  HALUJ_CHECK(board.test_and_clear(3));
  HALUJ_CHECK(!board.test(3));
  HALUJ_CHECK(board.raise(4));
}

static void test_set_and_wait()
{
  haluj::set_and_wait sw;

  int  sets     = 0;
  int  successes = 0;
  int  idles    = 0;
  bool ready    = false;

  auto step = [&]
  {
    sw([&] { ++sets; },
       [&] { return ready; },
       [&] { ++successes; },
       [&] { ++idles; });
  };

  step();               // set
  step();               // wait, not ready
  ready = true;
  step();               // wait, ready
  step();               // set again

  HALUJ_CHECK(sets == 2);
  HALUJ_CHECK(idles == 1);
  HALUJ_CHECK(successes == 1);
}

static void test_utility()
{
  int array[5] = {};
  static_assert(haluj::array_size(array) == 5, "array_size");

  haluj::async_loop<int> loop(0);
  int steps = 0;
  while (loop([](int v) { return v < 3; }, [](int v) { return v + 1; }))
    ++steps;

  HALUJ_CHECK(steps == 3);
  HALUJ_CHECK(loop.value_ == 3);
}

int main()
{
  test_bidirectional_map();
  test_digital_input_filter();
  test_blackboard();
  test_set_and_wait();
  test_utility();
  return haluj_test::report("test_utilities");
}
