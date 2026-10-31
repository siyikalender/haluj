/// \file test_utilities.cpp
/// Tests of pair_lookup, digital_input_filter, event_strategy,
/// set_and_wait and utility.
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
#include <cstring>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

#include "haluj/bounded/vector.hpp"
#include "haluj/digital_input_filter.hpp"
#include "haluj/event_strategy.hpp"
#include "haluj/pair_lookup.hpp"
#include "haluj/set_and_wait.hpp"
#include "haluj/utility.hpp"

#include "check.hpp"

static void test_pair_lookup()
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

// The same lookups give the same answers whatever holds the pairs
template<typename Range>
static void check_lookups_on(const Range& table)
{
  using haluj::to_first;
  using haluj::to_second;

  HALUJ_CHECK(std::strcmp(*to_second<std::optional>(1, table), "on") == 0);
  HALUJ_CHECK(std::strcmp(*to_second<std::optional>(2, table), "off") == 0);
  HALUJ_CHECK(!to_second<std::optional>(3, table).has_value());

  // Copies of the strings: only a comparison by content finds them
  char        on[]  = "on";
  char        off[] = "off";
  const char* key_on  = on;
  const char* key_off = off;

  HALUJ_CHECK(*to_first<std::optional>(key_on,  table, haluj::c_str_equal_to()) == 1);
  HALUJ_CHECK(*to_first<std::optional>(key_off, table, haluj::c_str_equal_to()) == 2);
  HALUJ_CHECK(!to_first<std::optional>("x", table, haluj::c_str_equal_to()).has_value());

  // With the default comparator the pointers are compared
  HALUJ_CHECK(!to_first<std::optional>(key_off, table).has_value());

  // The iterator overloads give the same answers
  HALUJ_CHECK(*to_first<std::optional>(key_off, std::begin(table), std::end(table),
                                       haluj::c_str_equal_to()) == 2);
  HALUJ_CHECK(std::strcmp(*to_second<std::optional>(2, std::begin(table), std::end(table)),
                          "off") == 0);
}

static void test_pair_lookup_ranges()
{
  using entry = std::pair<int, const char*>;

  static const entry c_array[] = {{1, "on"}, {2, "off"}};
  check_lookups_on(c_array);

  const std::array<entry, 2> std_array = {{{1, "on"}, {2, "off"}}};
  check_lookups_on(std_array);

  haluj::bounded::vector<entry, 4> bounded;
  bounded.push_back(entry(1, "on"));
  bounded.push_back(entry(2, "off"));
  check_lookups_on(bounded);

  const std::vector<entry> std_vector = {{1, "on"}, {2, "off"}};
  check_lookups_on(std_vector);
}

// The key need not have the type of the elements
static void test_pair_lookup_key_type()
{
  enum class mode { idle, run };

  static const std::pair<std::uint8_t, mode> modes[] = {{0, mode::idle}, {1, mode::run}};

  HALUJ_CHECK(*haluj::to_second<std::optional>(1, modes) == mode::run);
  HALUJ_CHECK(*haluj::to_first<std::optional>(mode::idle, modes) == 0);

  // A table of half open ranges, looked up with in_range
  static const std::pair<std::pair<int, int>, char> grades[] =
  {
    {{0, 50},   'F'},
    {{50, 70},  'C'},
    {{70, 101}, 'A'}
  };

  HALUJ_CHECK(*haluj::to_second<std::optional>(0,   grades, haluj::in_range()) == 'F');
  HALUJ_CHECK(*haluj::to_second<std::optional>(69,  grades, haluj::in_range()) == 'C');
  HALUJ_CHECK(*haluj::to_second<std::optional>(70,  grades, haluj::in_range()) == 'A');
  HALUJ_CHECK(!haluj::to_second<std::optional>(101, grades, haluj::in_range()).has_value());
}

static void test_digital_input_filter()
{
  haluj::digital_input_filter<unsigned, 3> f(0x1u);

  HALUJ_CHECK(f(1u) == 0u);
  HALUJ_CHECK(f(1u) == 0u);
  HALUJ_CHECK(f(1u) == 1u); // three equal samples
  HALUJ_CHECK(f.edge() == 1u);
  HALUJ_CHECK(f(1u) == 1u);
  HALUJ_CHECK(f.edge() == 0u);

  // Bits outside the mask are ignored
  haluj::digital_input_filter<unsigned, 2> g(0x1u);
  g(0x2u);
  HALUJ_CHECK(g(0x2u) == 0u);
  HALUJ_CHECK(g.mask() == 0x1u);
}

namespace
{

// Feeds a pattern of single bit samples and returns the outputs as a string
// of '0' and '1', one per sample
template<typename Debounce>
const char* debounce(const char* samples)
{
  static char out[32];

  haluj::digital_input_filter<unsigned, 4, Debounce> f(0x1u);

  std::size_t i = 0;
  for (; samples[i] != '\0'; ++i)
    out[i] = f(samples[i] == '1' ? 1u : 0u) ? '1' : '0';
  out[i] = '\0';

  return out;
}

bool is(const char* a, const char* b)
{
  return std::strcmp(a, b) == 0;
}

} // namespace

static void test_debounce_patterns()
{
  using namespace haluj;

  // A noisy sample while the input is held at 1
  const char* glitch_high = "11110111";
  HALUJ_CHECK(is(debounce<debounce_rising >(glitch_high), "00010000"));
  HALUJ_CHECK(is(debounce<debounce_falling>(glitch_high), "11111111"));
  HALUJ_CHECK(is(debounce<debounce_both   >(glitch_high), "00011111"));

  // A noisy sample while the input is held at 0, after a press
  const char* glitch_low = "111100001000";
  HALUJ_CHECK(is(debounce<debounce_rising >(glitch_low), "000100000000"));
  HALUJ_CHECK(is(debounce<debounce_falling>(glitch_low), "111111101111"));
  HALUJ_CHECK(is(debounce<debounce_both   >(glitch_low), "000111100000"));

  // Bouncing on press and release
  const char* bounce = "1010111110100000";
  HALUJ_CHECK(is(debounce<debounce_rising >(bounce), "0000000110000000"));
  HALUJ_CHECK(is(debounce<debounce_falling>(bounce), "1111111111111100"));
  HALUJ_CHECK(is(debounce<debounce_both   >(bounce), "0000000111111100"));

  // The default keeps the behaviour the filter always had
  HALUJ_CHECK(is(debounce<debounce_rising>(bounce),
                 []
                 {
                   static char out[32];
                   haluj::digital_input_filter<unsigned, 4> f(0x1u);
                   const char* in = "1010111110100000";
                   std::size_t i = 0;
                   for (; in[i] != '\0'; ++i)
                     out[i] = f(in[i] == '1' ? 1u : 0u) ? '1' : '0';
                   out[i] = '\0';
                   return out;
                 }()));
}

// Each policy against its definition, worked out from the last samples for
// every bit, on pseudo random input with a mask and several buffer sizes
template<typename Debounce, unsigned N>
static void check_debounce_model(int policy)
{
  const std::uint8_t mask = 0x5Au;

  haluj::digital_input_filter<std::uint8_t, N, Debounce> f(mask);

  std::uint8_t history[N] = {};
  std::uint8_t expected   = 0;
  std::uint32_t seed      = 12345u + N;

  for (int step = 0; step < 2000; ++step)
  {
    seed = seed * 1103515245u + 12345u;
    // Mostly steady inputs with occasional changes, so every policy has
    // something to filter
    const std::uint8_t raw = (seed >> 24) < 40u ? std::uint8_t(seed >> 8)
                                                : history[0] ^ std::uint8_t((seed >> 16) & 0x10u);

    for (unsigned k = N - 1; k > 0; --k)
      history[k] = history[k - 1];
    history[0] = std::uint8_t(raw & mask);

    std::uint8_t next = 0;
    for (unsigned bit = 0; bit < 8; ++bit)
    {
      const std::uint8_t b = std::uint8_t(1u << bit);
      if ((mask & b) == 0)
        continue;

      unsigned high = 0;
      for (unsigned k = 0; k < N; ++k)
        high += (history[k] & b) ? 1u : 0u;

      bool on = false;
      switch (policy)
      {
        case 0: on = (high == N);                                      break; // rising
        case 1: on = (high > 0);                                       break; // falling
        case 2: on = (high == N) || (high > 0 && (expected & b) != 0); break; // both
      }
      if (on)
        next = std::uint8_t(next | b);
    }

    const std::uint8_t previous = expected;
    expected = next;

    HALUJ_CHECK(f(raw) == expected);
    HALUJ_CHECK(f.edge() == std::uint8_t(previous ^ expected));
  }
}

template<unsigned N>
static void check_debounce_models()
{
  check_debounce_model<haluj::debounce_rising,  N>(0);
  check_debounce_model<haluj::debounce_falling, N>(1);
  check_debounce_model<haluj::debounce_both,    N>(2);
}

static void test_debounce_models()
{
  check_debounce_models<1>();
  check_debounce_models<2>();
  check_debounce_models<3>();
  check_debounce_models<5>();
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
  test_pair_lookup();
  test_pair_lookup_ranges();
  test_pair_lookup_key_type();
  test_digital_input_filter();
  test_debounce_patterns();
  test_debounce_models();
  test_blackboard();
  test_set_and_wait();
  test_utility();
  return haluj_test::report("test_utilities");
}
