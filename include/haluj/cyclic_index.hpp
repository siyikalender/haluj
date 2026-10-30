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

#include <type_traits>

namespace haluj
{

/// Advances v by one step in a ring of N. Requires v < N.
template<typename T>
constexpr T cyclic_increment(const T v, const T N)
{
  return (v == T(N - 1)) ? T(0) : T(v + 1);
}

/// Moves v back by one step in a ring of N. Requires v < N.
template<typename T>
constexpr T cyclic_decrement(const T v, const T N)
{
  return (v == T(0)) ? T(N - 1) : T(v - 1);
}

/// Advances v by size steps in a ring of N. Requires v < N and size <= N.
/// v + size is never formed, so it cannot overflow T.
template<typename T>
constexpr T cyclic_increment(const T v, T size, const T N)
{
  return (v >= T(N - size)) ? T(v - (N - size)) : T(v + size);
}

/// Moves v back by size steps in a ring of N. Requires v < N and size <= N.
template<typename T>
constexpr T cyclic_decrement(const T v, T size, const T N)
{
  return (v >= size) ? T(v - size) : T(v + (N - size));
}

/// Number of cyclic_increment steps that take from to to in a ring of N,
/// that is (to - from) mod N. Requires from < N and to < N.
///
/// For example the number of elements between the tail and the head of a
/// ring buffer is cyclic_distance_forward(tail, head, N).
template<typename T>
constexpr T cyclic_distance_forward(const T from, const T to, const T N)
{
  return (to >= from) ? T(to - from) : T(N - (from - to));
}

/// Number of cyclic_decrement steps that take from to to in a ring of N,
/// that is (from - to) mod N. Requires from < N and to < N.
///
/// cyclic_distance_backward(a, b, N) == cyclic_distance_forward(b, a, N)
template<typename T>
constexpr T cyclic_distance_backward(const T from, const T to, const T N)
{
  return cyclic_distance_forward(to, from, N);
}


/// An index that runs round a ring of N positions: 0, 1, ..., N - 1, 0, ...
///
/// It is a plain value: trivially copyable, and usable in constant
/// expressions. value is kept below N by the operators; code that writes
/// value directly must keep it below N too.
template<typename T, T N>
struct cyclic_index
{
  static_assert(std::is_integral<T>::value, "cyclic_index needs an integral type");
  static_assert(N > T(0), "cyclic_index needs at least one position");

  constexpr cyclic_index& operator++()
  {
    value = cyclic_increment(value, N);
    return *this;
  }

  constexpr cyclic_index operator++(int)
  {
    cyclic_index result = *this;
    value = cyclic_increment(value, N);
    return result;
  }

  constexpr cyclic_index& operator--()
  {
    value = cyclic_decrement(value, N);
    return *this;
  }

  constexpr cyclic_index operator--(int)
  {
    cyclic_index result = *this;
    value = cyclic_decrement(value, N);
    return result;
  }

  constexpr T operator*() const
  {
    return value;
  }

  constexpr cyclic_index& assign(const cyclic_index& p_other)
  {
    value = p_other.value;
    return *this;
  }

  constexpr void reset()
  {
    value = T(0);
  }

  T   value = T(0);
};

// The comparisons compare the raw positions. On a ring "less than" is not an
// order of the positions in time; use cyclic_distance_forward for that.

template<typename T, T N>
constexpr bool operator<(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value < B.value;
}

template<typename T, T N>
constexpr bool operator==(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value == B.value;
}

template<typename T, T N>
constexpr bool operator>(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value > B.value;
}

template<typename T, T N>
constexpr bool operator!=(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value != B.value;
}

template<typename T, T N>
constexpr bool operator<=(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value <= B.value;
}

template<typename T, T N>
constexpr bool operator>=(cyclic_index<T, N> A, cyclic_index<T, N> B)
{
  return A.value >= B.value;
}

} // namespace haluj

#endif // HALUJ_CYCLIC_INDEX_HPP
