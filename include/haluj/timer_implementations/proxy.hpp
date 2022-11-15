/// \file timer_proxy.hpp
/// timer proxy for static timer implementations (e.g hardware timers)
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

#ifndef HALUJ_TIMER_PROXY_HPP
#define HALUJ_TIMER_PROXY_HPP

namespace haluj
{

template<typename Timer>
timer_proxy
{
  static constexpr bool auto_reset = Timer::auto_reset;
  
  void load(duration p_value) 
  {
    Timer::load(p_value);
  }

  void start()
  {
    Timer::start();
  }
  
  void stop()
  {
    Timer::stop();
  }
  
  bool is_running() const
  {
    Timer::is_running();
  }

  void iterate(duration p_delta)
  {
    Timer::iterate(p_delta);
  }

  bool predicate() const
  {
    return Timer::predicate();
  }

  void reset()
  {
    Timer::reset();
  }  
  
  bool operator ()(duration p_delta)
  {
    return Timer::proceed(p_delta);
  }
  
};

} // namespace haluj

// HALUJ_TIMER_PROXY_HPP
#endif
