/// \file chrono.hpp
/// std chrono based timer implementation
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

namespace timer_implementations
{

template
<
  typename  Clock, 
  typename  Duration,
  bool      AutoReset
>
struct chrono
{
  typedef typename Clock::time_point  time_point;
  typedef Duration                    duration;
  
  static constexpr bool auto_reset = AutoReset;

  chrono() {}
  
  chrono(const chrono& ) = delete;
  
  chrono(chrono&& ) = delete;

  void load(duration p_value) 
  {
    load_ = p_value;
  }

  void start()
  {
    run_ = true;
  }
  
  void stop()
  {
    run_ = false;;
  }
  
  bool is_running() const
  {
    return run_;
  }

  void iterate(duration p_delta)
  {}

  bool predicate() const
  {
    return Clock::now() >= timeout_;
  }

  void reset()
  {
    timeout_ = Clock::now() + load_;
  }  
  
  bool operator ()(duration)
  {
    bool result = run_ && predicate();
    
    if (auto_reset && result)
    {
      timeout_ += load_;
    }
    
    return result;
  }
  
  bool        run_;
  time_point  timeout_;
  duration    load_;
};

} // namespace timer_implementations

} // namespace haluj

// HALUJ_TIMER_PROXY_HPP
#endif
