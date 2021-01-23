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

#ifndef HALUJ_TIMER_HPP
#define HALUJ_TIMER_HPP

namespace haluj
{

struct periodic
{
  bool operator()() { return false; };

  template<typename Function> 
  bool operator()(Function f) { f(); return false; };
};

struct one_shot
{
  bool operator()() { return true; };

  template<typename Function> 
  bool operator()(Function f) { f(); return true; };
};

template<typename Implementation>
struct timer
{
  typedef Implementation                    implementation;
  typedef typename implementation::duration duration;
  
  // Constructors

  timer()
  {}

  timer(const timer& ) = delete;
  
  timer(timer&& ) = delete;

  // Operators

  template<typename Function = periodic>
  bool operator()(duration  p_delta,
                  Function  p_function = Function())
  {
    bool result = impl_(p_delta);
    
    if (result)
    {
      if (p_function())
      {
        impl_.stop();
      }      
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
};

template<typename Implementation>
struct static_timer
{
  typedef Implementation                    implementation;
  typedef typename implementation::duration duration;
  
  // Constructors
private:
  
  static_timer()
  {}

  static_timer(const static_timer& ) = delete;
  
  static_timer(static_timer&& ) = delete;

public:

  // Operators

  template<typename Function = periodic> 
  static bool poll(duration   p_delta     = duration(),
                   Function   p_function  = Function())
  {
    bool result = implementation::proceed(p_delta);
    
    if (result)
    {
      if (p_function())
      {
        implementation::stop();
      }
    }
    
    return result;
  }

  // Methods
  
  static void set(duration p_value)
  {
    implementation::load(p_value);
    implementation::reset();
    implementation::start();
  }
  
  static bool is_running()
  {
    return implementation::is_running();
  }

};

} // namespace haluj

// HALUJ_TIMER_HPP
#endif
