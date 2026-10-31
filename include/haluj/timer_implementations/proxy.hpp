/// \file proxy.hpp
/// Timer implementation that forwards to a timer made of static functions,
/// for example a hardware timer
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

/// \author Selcuk Iyikalender
/// \date   2022

#ifndef HALUJ_TIMER_IMPLEMENTATIONS_PROXY_HPP
#define HALUJ_TIMER_IMPLEMENTATIONS_PROXY_HPP

#include <concepts>

namespace haluj
{

namespace timer_implementations
{

/// Adapts a timer made of static member functions, typically a hardware
/// timer, to the implementation interface that timer<> uses:
///
///   haluj::timer<haluj::timer_implementations::proxy<tim2>> t;
///
/// Timer must provide (the static_timer concept below checks it):
///
///   typedef ... duration;
///   static void load(duration);       the timeout
///   static void start();
///   static void stop();
///   static bool is_running();
///   static void reset();              start over: a full timeout from now
///   static void reload();             start the next period, keeping the
///                                     time that ran past the timeout
///   static bool proceed(duration);    advance by delta (a hardware timer
///                                     may ignore it); true once the timeout
///                                     is reached
///
/// The proxy holds no state, so every timer<proxy<Timer>> object drives the
/// same Timer.
template<typename Timer>
concept static_timer =
  requires(typename Timer::duration delta)
  {
    Timer::load(delta);
    Timer::start();
    Timer::stop();
    { Timer::is_running() } -> std::convertible_to<bool>;
    Timer::reset();
    Timer::reload();
    { Timer::proceed(delta) } -> std::convertible_to<bool>;
  };

template<static_timer Timer>
struct proxy
{
  typedef typename Timer::duration duration;

  constexpr void load(duration p_value)
  {
    Timer::load(p_value);
  }

  constexpr void start()
  {
    Timer::start();
  }

  constexpr void stop()
  {
    Timer::stop();
  }

  constexpr bool is_running() const
  {
    return Timer::is_running();
  }

  constexpr void reset()
  {
    Timer::reset();
  }

  constexpr void reload()
  {
    Timer::reload();
  }

  constexpr bool operator ()(duration p_delta)
  {
    return Timer::proceed(p_delta);
  }
};

} // namespace timer_implementations

} // namespace haluj

// HALUJ_TIMER_IMPLEMENTATIONS_PROXY_HPP
#endif
