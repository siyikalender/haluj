/// \file test_timer.cpp
/// Tests of the timer and its software and chrono implementations.
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

#include <chrono>

#include "haluj/timer.hpp"
#include "haluj/timer_implementations/chrono.hpp"
#include "haluj/timer_implementations/software.hpp"

#include "check.hpp"

namespace sw = haluj::timer_implementations::software;

static void test_periodic()
{
  haluj::timer<sw::fwd<unsigned>> t;
  HALUJ_CHECK(!t.is_running());

  t.set(3);
  HALUJ_CHECK(t.is_running());

  int fired = 0;
  int calls = 0;
  for (int i = 0; i < 9; ++i)
  {
    if (t([&calls] { ++calls; }, 1u))
      ++fired;
  }
  HALUJ_CHECK(fired == 3);
  HALUJ_CHECK(calls == 3);
  HALUJ_CHECK(t.is_running());

  t.stop();
  HALUJ_CHECK(!t(nullptr, 10u));
}

static void test_one_shot()
{
  haluj::timer<sw::fwd<unsigned>, haluj::one_shot> t;
  t.set(2);

  int fired = 0;
  for (int i = 0; i < 6; ++i)
    fired += t(nullptr, 1u) ? 1 : 0;

  HALUJ_CHECK(fired == 1);
  HALUJ_CHECK(!t.is_running());
}

static void test_user_behaviour()
{
  // The callback decides whether the timer keeps running
  haluj::timer<sw::fwd<unsigned>, haluj::user> t;
  t.set(1);

  int fired = 0;
  for (int i = 0; i < 10; ++i)
    t([&fired] { return ++fired == 3; }, 1u);

  HALUJ_CHECK(fired == 3);
  HALUJ_CHECK(!t.is_running());
}

static void test_backward()
{
  haluj::timer<sw::bwd<int>> t;
  t.set(5);

  int first = -1;
  for (int i = 1; i <= 10 && first < 0; ++i)
  {
    if (t(nullptr, 1))
      first = i;
  }
  HALUJ_CHECK(first == 5);
}

static void test_chrono()
{
  using clock = std::chrono::steady_clock;
  using ms    = std::chrono::milliseconds;

  haluj::timer<haluj::timer_implementations::chrono<clock, ms, true>> t;
  t.stop();
  t.set(ms(1000));
  HALUJ_CHECK(t.is_running());
  HALUJ_CHECK(!t(nullptr, 0)); // a second has not passed yet

  t.set(ms(0));
  HALUJ_CHECK(t(nullptr, 0));
}

int main()
{
  test_periodic();
  test_one_shot();
  test_user_behaviour();
  test_backward();
  test_chrono();
  return haluj_test::report("test_timer");
}
