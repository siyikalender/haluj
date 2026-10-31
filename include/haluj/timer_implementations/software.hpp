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


#ifndef HALUJ_TIMER_IMPLEMENTATIONS_SOFTWARE_HPP
#define HALUJ_TIMER_IMPLEMENTATIONS_SOFTWARE_HPP

namespace haluj
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
  
/// Counts the elapsed time up to the timeout.
///
/// The counter reaches at most timeout + delta - 1 before it is reloaded, so
/// that value must fit in the duration type. This matters for small types:
/// with uint8_t, timeout + delta must not exceed 256.
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

  /// Starts over from zero
  void reset()
  {
    elapsed_ = duration(0);
  }

  /// Starts the next period, keeping the time that ran past the timeout
  void reload()
  {
    elapsed_ -= base_t::timeout_;
  }
  
  duration    elapsed_ = duration(0);
};

/// Counts the remaining time down to zero.
///
/// The duration type must be signed (int32_t, float, a std::chrono duration
/// with a signed representation...): a step larger than the remaining time
/// takes it below zero, which an unsigned type would wrap to a large value
/// and never expire.
template<typename DurationType>
struct backward : base<DurationType>
{
  static_assert(DurationType(-1) < DurationType(0),
                "backward timer needs a signed duration type");

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
  
  /// Starts over from the full timeout
  void reset()
  {
    remaining_ = base_t::timeout_;
  }

  /// Starts the next period, keeping the time that ran past zero
  void reload()
  {
    remaining_ += base_t::timeout_;
  }
  
  duration    remaining_ = duration(0);
};

/// A software timer advanced by the caller with operator()(delta).
///
/// With AutoReset, an expiry starts the next period without losing the time
/// that ran past it, so a step that does not divide the timeout does not
/// make the period drift. A step longer than the timeout makes the timer
/// fall behind: it expires once per call until it has caught up. Keep the
/// step no longer than the timeout.
template<typename DirectionType, bool AutoReset = true>
struct implementation : DirectionType
{
  typedef DirectionType                 direction;
  typedef typename direction::duration  duration;
  
  static constexpr bool auto_reset = AutoReset;

  bool operator ()(duration p_delta)
  {
    bool result = false;
    
    if (direction::is_running())
    {
      direction::iterate(p_delta);
      
      result = direction::predicate();

      if(auto_reset && result)
      {
        direction::reload();
      }
    }
    
    return result;
  }
};

template<typename DurationType>
using fwd = implementation<forward<DurationType>>;

template<typename DurationType>
using bwd = implementation<backward<DurationType>>;

} // namespace software

} // namespace timer_implementations

} // namespace haluj

#endif // HALUJ_TIMER_IMPLEMENTATIONS_SOFTWARE_HPP
