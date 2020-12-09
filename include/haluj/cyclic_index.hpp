/// \file cyclic_index.hpp
/// A cyclic index implementation
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

#ifndef HALUJ_CYCLIC_INDEX_HPP
#define HALUJ_CYCLIC_INDEX_HPP

namespace haluj
{

template<typename T>
inline T cyclic_increment(const T v, const T N)
{
  return (v == (N - 1)) ? T(0) : (v + 1);
}

template<typename T>
inline T cyclic_decrement(const T v, const T N)
{
  return (v == 0) ? (N - 1) : (v - 1);
}

template<typename T, T N>
struct cyclic_index
{
  // T should be an IntegralType
  
  cyclic_index() 
  : value(0) 
  {}
  
  cyclic_index(const cyclic_index& p_other)
  {
    assign(p_other);
  }
  
  cyclic_index& operator++()
  {
    value = cyclic_increment(value, N);
    return *this;
  }

  cyclic_index operator++(int)
  {
    cyclic_index result = *this;
    value = cyclic_increment(value, N);
    return result;
  }
  
  cyclic_index& operator--()
  {
    value = cyclic_decrement(value, N);
    return *this;
  }

  cyclic_index operator--(int)
  {
    cyclic_index result = *this;
    value = cyclic_decrement(value, N);
    return result;
  }  

  T operator*() const
  {
    return value;
  }

  cyclic_index& operator=(cyclic_index& p_other)
  {
    return assign(p_other);
  }
  
  cyclic_index& assign(const cyclic_index& p_other)
  {
    value = p_other.value;
    return *this;
  }
  
  void reset()
  {
    value = 0;
  }

  T   value;
};

template<typename T, T N>
inline bool operator<(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value < B.value;
}

template<typename T, T N>
inline bool operator==(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value == B.value;
}

// The functions below will be deprecated in C++20

template<typename T, T N>
inline bool operator>(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value > B.value;
}

template<typename T, T N>
inline bool operator!=(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value != B.value;
}

template<typename T, T N>
inline bool operator<=(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value <= B.value;
}

template<typename T, T N>
inline bool operator>=(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value >= B.value;
}

} // namespace haluj

#endif // HALUJ_CYCLIC_INDEX_HPP
