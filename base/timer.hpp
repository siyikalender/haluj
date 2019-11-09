/// \file timer.hpp
/// abstract timer implementation
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

#ifndef HALUJ_BASE_TIMER_HPP
#define HALUJ_BASE_TIMER_HPP

namespace haluj
{

namespace base
{

struct default_function
{
  void operator()() {};
};

struct one_shot
{
  template<typename Implementation>
  void operator()(Implementation& p_impl)
  {
    p_impl.stop();
  }
};

struct periodic
{
  template<typename Implementation>
  void operator()(Implementation& )
  {}
};

template<std::size_t N>
struct n_shot
{
  template<typename Implementation>
  void operator()(Implementation& p_impl)
  {
    current ++;
    if (current >= N)
    {
      p_impl.stop();
    }
  }

  std::size_t   current = 0;
};


template<typename Implementation, typename Continuity>
struct timer
{
  typedef Implementation                    implementation;
  typedef Continuity                        continuity;
  typedef typename implementation::duration duration;
  
  // Constructors

  timer()
  {}

  timer(const timer& ) = delete;
  
  timer(timer&& ) = delete;

  // Operators

  template<typename Function = default_function> 
  bool operator()(duration  p_delta,
                  Function  p_function = Function())
  {
    bool result = impl_(p_delta);
    
    if (result)
    {
      cont_(impl_);
      p_function();
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
  
  bool is_running() const
  {
    return impl_.is_running();
  }  

// private:

  implementation  impl_;
  continuity      cont_;

};

// helpers
template<typename T, typename D = T>
struct delta
{
  delta(T p_initial)
  : previous_(p_initial)
  {}
  
  D operator()(T current)
  {
    D result  = current - previous_;  
    previous_ = current;
    return result;
  }
  T     previous_;
};

} // namespace base

} // namespace haluj

// HALUJ_BASE_TIMER_HPP
#endif
