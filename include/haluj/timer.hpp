/// \file timer.hpp
/// abstract timer. Use implementations for specialization 
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

#ifndef HALUJ_TIMER_HPP
#define HALUJ_TIMER_HPP

#include <cstddef>

namespace haluj
{

// Timer behaviours
//
// A behaviour decides what happens when the timer expires. timer calls it
// with its implementation and with the function given to operator() (or
// nullptr when there is none). A behaviour of your own needs the same two
// call operators; through the implementation it can reload(), stop(),
// load() a new timeout and so on.

namespace timer_detail
{

inline void call(std::nullptr_t) {}

template<typename Function>
inline void call(Function& p_function) { p_function(); }

} // namespace timer_detail

/// Expires every timeout. The next period starts before the function is
/// called, so the function may call set() to start a different one.
struct periodic
{
  template<typename Implementation, typename Function>
  void operator()(Implementation& p_impl, Function& p_function) const
  {
    p_impl.reload();
    timer_detail::call(p_function);
  }
};

/// Expires once, then the timer stops until the next set()
struct one_shot
{
  template<typename Implementation, typename Function>
  void operator()(Implementation& p_impl, Function& p_function) const
  {
    p_impl.stop();
    timer_detail::call(p_function);
  }
};

// Timer 

template
<
  typename Implementation,
  typename Behaviour = periodic
>
struct timer
{
  // Types

  typedef Implementation                    implementation;
  typedef typename implementation::duration duration;
  typedef Behaviour                         behaviour;

  // Constructors

  timer()
  {}

  timer(const timer& ) = delete;
  
  timer(timer&& ) = delete;

  // Operators

  template
  <
    typename Function     = std::nullptr_t,
    typename DurationType = int
  >
  bool operator()
  (
    Function      p_function  = Function(),
    DurationType  p_delta     = DurationType(1)
  )
  {
    bool result = impl_(duration(p_delta));

    if (result)
    {
      behaviour_(impl_, p_function);
    }

    return result;
  }

  // Methods
  
  void set(duration p_value)
  {
    impl_.load(p_value);
    impl_.reset();
    impl_.start();
  }

  void load(duration p_value)
  {
    impl_.load(p_value);
  }
  
  void stop()
  {
    impl_.stop();
  }
  
  bool is_running() const
  {
    return impl_.is_running();
  }  

// private:

  implementation  impl_;
  behaviour       behaviour_;
};

} // namespace haluj

// HALUJ_TIMER_HPP
#endif
