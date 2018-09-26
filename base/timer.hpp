/**
 ** Configurable Real Time Controller, Core
 **
 ** Copyright (c) 2016, C-RTC Project Team
 **
 ** This software is provided 'as-is', without any express or implied
 ** warranty. In no event will the authors be held liable for any damages
 ** arising from the use of this software.
 **
 ** Permission is granted to anyone to use this software for any purpose,
 ** including commercial applications, and to alter it and redistribute it
 ** freely, subject to the following restrictions:
 **
 ** 1. The origin of this software must not be misrepresented; you must not
 **    claim that you wrote the original software. If you use this software
 **    in a product, an acknowledgement in the product documentation would be
 **    appreciated but is not required.
 ** 2. Altered source versions must be plainly marked as such, and must not be
 **    misrepresented as being the original software.
 ** 3. This notice may not be removed or altered from any source distribution.
 **
 ** Some sources in the project may have different licensing conditions
 **
 ** Authors:
 **
 ** Selcuk Iyikalender
 **
 **/

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
