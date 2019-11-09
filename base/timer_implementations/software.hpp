/// \file software.hpp
/// software implementation for timer
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
/// \date   2019


#ifndef HALUJ_BASE_TIMER_IMPLEMENTATIONS_SOFTWARE_HPP
#define HALUJ_BASE_TIMER_IMPLEMENTATIONS_SOFTWARE_HPP

namespace haluj
{

namespace base
{

namespace timer_implementations
{

namespace software
{

template<typename DurationType>
struct base
{
  typedef DurationType  duration;

  void load(duration p_value) 
  {
    timeout_ = p_value;
  }

  void start()
  {
    run_ = true;
  }
  
  void stop()
  {
    run_ = false;
  }
  
  bool is_running() const
  {
    return run_;
  }

  bool        run_      = false;
  duration    timeout_  = duration(0);
};
  
template<typename DurationType>
struct forward : base<DurationType>
{
  typedef base<DurationType>          base_t;
  typedef typename base_t::duration   duration;
  
  void iterate(duration p_delta)
  {
    elapsed_ += p_delta;
  }

  bool predicate() const
  {
    return (elapsed_ >= base_t::timeout_);
  }

  void reset()
  {
    elapsed_ = duration(0);
  }
  
  duration    elapsed_;
};

template<typename DurationType>
struct backward : base<DurationType>
{
  typedef base<DurationType>          base_t;
  typedef typename base_t::duration   duration;

  void iterate(duration p_delta)
  {
    remaining_ -= p_delta;
  }

  bool predicate() const
  {
    return (remaining_ <= duration(0));
  }
  
  void reset()
  {
    remaining_ = base_t::timeout_;
  }
  
  duration    remaining_;
};

template<typename DirectionType>
struct implementation : DirectionType
{
  typedef DirectionType                 direction;
  typedef typename direction::duration  duration;

  bool operator ()(duration p_delta)
  {
    bool result = false;
    
    if (direction::is_running())
    {
      direction::iterate(p_delta);
      if (direction::predicate())
      {
        direction::reset();
        result = true;
      }
    }
    
    return result;
  }
};

} // namespace software

} // namespace timer_implementations

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_TIMER_IMPLEMENTATIONS_SOFTWARE_HPP
