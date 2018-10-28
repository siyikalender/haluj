/// \file set_and_wait.hpp
/// asynchronous implementation for set and wait
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

#ifndef HALUJ_BASE_SET_AND_WAIT_HPP
#define HALUJ_BASE_SET_AND_WAIT_HPP

namespace haluj
{

namespace base
{

struct set_and_wait
{
  struct default_function { void operator()() const {} };

  struct default_test_function { bool operator()() const { return true; } };
  
  enum class states {
    set,
    wait
  };
  
  set_and_wait()
  : state_(states::set)
  {}
  
  template<typename SetFunction       = default_function, 
           typename TestFunction      = default_test_function, 
           typename SuccessFunction   = default_function,
           typename WaitIdleFunction  = default_function>
  void operator () (SetFunction       set_function        = SetFunction(),
                    TestFunction      test_function       = TestFunction(),
                    SuccessFunction   success_function    = SuccessFunction(),
                    WaitIdleFunction  wait_idle_function  = WaitIdleFunction())
  {
    switch(state_)
    {
      case states::set:
        set_function();
        state_ = states::wait;
        break;
      case states::wait:
      default:
        if (test_function())
        {
          success_function();
          state_ = states::set;
        }
        else
        {
          wait_idle_function();
        }
        break;
    }
  }

  states state_;
};

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_SET_AND_WAIT_HPP
