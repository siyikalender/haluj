/// \file options.hpp
/// a generic options mechanism used for peripheral configuration
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

#ifndef HALUJ_BASE_OPTIONS_HPP
#define HALUJ_BASE_OPTIONS_HPP

namespace haluj
{

namespace base
{

template<typename ... Types>
struct options_t
{
  template<typename R, typename T, typename Operation>
  R accept(T, Operation op)
  {
    return R();
  }
};

template<typename OptionType, typename ... Options>
struct options_t<OptionType, Options...>
{
  typedef options_t<Options...> next;

  options_t(const OptionType& p_option, Options... opts)
  : option_(p_option), next_(opts...)
  {}

  template<typename R, 
           typename T, 
           typename Operation>
  R accept(T, Operation op)
  {
    return 
      op.template proceed<R>(
        option_.accept(T()), 
        next_.template accept<R>(T(), op)
      );
  }

  OptionType    option_;
  next          next_;
};

template<typename OptionType>
options_t<OptionType>
options(const OptionType& p_a)
{
  return options_t<OptionType>(p_a);
}

template<typename OptionType, typename ... Types>
options_t<OptionType, Types...>
options(const OptionType& p_a, Types... args)
{
  return options_t<OptionType, Types...>(p_a, args...);
}

// Operations can be used 

struct null_op
{
  template <typename R, typename A, typename B>
  R proceed(A, B) { return R(); }
};

struct or_op
{
  template <typename R, typename A, typename B>
  R proceed(A a, B b) { return a | b; }
};

} // namespace base

} // namespace haluj

// HALUJ_BASE_OPTIONS_HPP
#endif 
