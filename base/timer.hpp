/// \file timer.hpp
/// abstract software timers
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
/// \date   2018

#ifndef TIMER_HPP
#define TIMER_HPP

struct default_timer_function
{
  void operator()() {};
};

template<typename TimePointType>
struct base_timer
{
  typedef TimePointType       time_point;

  base_timer()
  : run_(false),
    timeout_()
  {}
  
  void set(const time_point&  p_timeout)
  {
    timeout_ = p_timeout;
  }
  
  template<typename DurationType>
  time_point advance(const DurationType& p_interval)
  {
    timeout_ += p_interval;
    
    return timeout_;
  }
  
  void start()
  {
    run_ = true;
  }
  
  void stop()
  {
    run_ = false;
  }
  
  bool test_timeout(const time_point& p_now) const
  {
    return run_ && (p_now >= timeout_);
  }
  
  bool is_running() const
  {
    return run_;
  }

  bool              run_;
  time_point        timeout_;
};

template<typename TimePointType>
struct one_shot_timer 
: base_timer<TimePointType>
{
  typedef base_timer<TimePointType> base;
  
  one_shot_timer()
  : base()
  {}

  template<typename Function = default_timer_function> 
  bool operator()(const typename base::time_point&    p_now,
                  Function                            p_function = Function())
  {
    bool result = false;
    
    if (base::test_timeout(p_now))
    {
      base::stop();
      p_function();
      result = true;
    }
    
    return result;
  }

};

template<typename TimePointType,
         typename DurationType>
struct periodic_timer 
: base_timer<TimePointType>
{
  typedef DurationType                duration;
  typedef base_timer<TimePointType>   base;
 
  periodic_timer(const duration&                    p_period)
  : base(),
    period(p_period)
  {}

  void start(const typename base::time_point& p_now)
  {
    base::set(p_now + period);
    base::start();
  }

  template<typename Function = default_timer_function> 
  bool operator()(const typename base::time_point& p_now,
                  Function                         p_function = Function())
  {
    bool result = false;
    
    if (base::test_timeout(p_now))
    {
      base::advance(period);
      p_function();
      result  =   true;
    }
    
    return result;
  }

  duration      period;
};



// TIMER_HPP
#endif
