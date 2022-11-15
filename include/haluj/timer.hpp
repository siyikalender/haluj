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

namespace haluj
{

// Timer behaviours

struct periodic
{
  template<typename Function> 
  bool operator()(Function f) { f(); return false; };
};

struct one_shot
{
  template<typename Function> 
  bool operator()(Function f) { f(); return true; };
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
    typename Function /* = null_function */
    , typename DurationType = int
  >
  bool operator()
  (
    Function      p_function,
    DurationType  p_delta = DurationType(1)
  )
  {
    bool result = impl_(duration(p_delta));
    
    if (result && behaviour_(p_function))
    {
      // stop timer for one or N shot behaviour
      impl_.stop();
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
