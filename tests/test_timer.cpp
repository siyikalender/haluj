/// \file test_timer.cpp
/// Tests of the timer and its software and chrono implementations.
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
#include <cstdint>

#include "haluj/timer.hpp"

// chrono.hpp used to share its include guard with proxy.hpp, so after
// proxy.hpp it was skipped. Defining the old guard here makes sure it no
// longer depends on that name.
#define HALUJ_TIMER_PROXY_HPP
#include "haluj/timer_implementations/chrono.hpp"
#undef HALUJ_TIMER_PROXY_HPP
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

// A step larger than the remaining time takes a signed counter below zero,
// which still expires: 5, 3, 1, -1
template<typename Duration>
static int first_backward_expiry(Duration timeout, Duration step)
{
  haluj::timer<sw::bwd<Duration>, haluj::one_shot> t;
  t.set(timeout);

  for (int i = 1; i <= 10; ++i)
  {
    if (t(nullptr, step))
      return i;
  }
  return -1;
}

static void test_backward_large_step()
{
  HALUJ_CHECK(first_backward_expiry<std::int8_t>(5, 2) == 3);
  HALUJ_CHECK(first_backward_expiry<std::int32_t>(5, 2) == 3);
  HALUJ_CHECK(first_backward_expiry<float>(5.0f, 2.0f) == 3);
  HALUJ_CHECK(first_backward_expiry<std::chrono::milliseconds>(
                std::chrono::milliseconds(5), std::chrono::milliseconds(2)) == 3);

  // A step that lands exactly on zero
  HALUJ_CHECK(first_backward_expiry<int>(6, 2) == 3);
}

// A periodic timer must not drift when the step does not divide the period:
// after every call the number of expiries equals the number of whole periods
// in the time passed so far
template<typename Implementation>
static void check_no_drift(int period, int step, int calls)
{
  using duration = typename Implementation::duration;

  haluj::timer<Implementation> t;
  t.set(duration(period));

  int fired = 0;
  for (int i = 1; i <= calls; ++i)
  {
    if (t(nullptr, duration(step)))
      ++fired;

    HALUJ_CHECK(fired == (i * step) / period);
  }
}

static void test_periodic_no_drift()
{
  for (int period = 1; period <= 9; ++period)
  {
    for (int step = 1; step <= period; ++step)
    {
      check_no_drift<sw::fwd<unsigned>>(period, step, 200);
      check_no_drift<sw::fwd<int>>(period, step, 200);
      check_no_drift<sw::bwd<int>>(period, step, 200);
      check_no_drift<sw::bwd<std::int16_t>>(period, step, 200);
    }
  }

  // A small type at its limit: timeout + step = 256
  check_no_drift<sw::fwd<std::uint8_t>>(200, 56, 500);

  // The case from the review: period 3, step 2, 24 time units
  haluj::timer<sw::fwd<unsigned>> t;
  t.set(3u);
  int fired = 0;
  for (int i = 0; i < 12; ++i)
    fired += t(nullptr, 2u) ? 1 : 0;
  HALUJ_CHECK(fired == 8);
}

// A step longer than the period loses no period: the timer expires once per
// call until it has caught up
template<typename Implementation>
static void check_catch_up()
{
  using duration = typename Implementation::duration;

  haluj::timer<Implementation> t;
  t.set(duration(3));

  int fired = t(nullptr, duration(10)) ? 1 : 0;
  for (int i = 0; i < 5; ++i)
    fired += t(nullptr, duration(0)) ? 1 : 0;

  HALUJ_CHECK(fired == 3); // 10 / 3 whole periods
}

static void test_periodic_catch_up()
{
  check_catch_up<sw::fwd<unsigned>>();
  check_catch_up<sw::bwd<int>>();
}

// set() starts a full period again, whatever the counter held
static void test_set_restarts()
{
  haluj::timer<sw::fwd<unsigned>> f;
  f.set(5u);
  f(nullptr, 4u);
  f.set(5u);
  HALUJ_CHECK(!f(nullptr, 4u));
  HALUJ_CHECK(f(nullptr, 1u));

  haluj::timer<sw::bwd<int>> b;
  b.set(5);
  b(nullptr, 4);
  b.set(5);
  HALUJ_CHECK(!b(nullptr, 4));
  HALUJ_CHECK(b(nullptr, 1));
}

static void test_chrono()
{
  using clock = std::chrono::steady_clock;
  using ms    = std::chrono::milliseconds;

  haluj::timer<haluj::timer_implementations::chrono<clock, ms, true>> t;
  HALUJ_CHECK(!t.is_running()); // not started before set()

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
  test_backward_large_step();
  test_periodic_no_drift();
  test_periodic_catch_up();
  test_set_restarts();
  test_chrono();
  return haluj_test::report("test_timer");
}
