/// \file bidirectional_map.hpp
/// A map implementation where elements can be accessed from both pair arguments
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
#ifndef HALUJ_BIDIRECTIONAL_MAP_HPP
#define HALUJ_BIDIRECTIONAL_MAP_HPP

#include <utility>
#include <iterator>
#include <functional>
#include <algorithm>
#include <cstring>

namespace haluj
{

struct c_str_equal_to
{
  bool operator ()(const char* a, const char* b)
  {
    return strcmp(a, b) == 0;
  }

  constexpr bool operator ()(const char* a, const char* b) const
  {
    return strcmp(a, b) == 0;
  }
};

struct in_range
{
  template<typename T>
  bool operator ()(const std::pair<T, T>& a, const T& b) const
  {
    return (b >= a.first) && (b < a.second);
  }
};

template<typename     T,
         typename     Iterator,
         typename     CompFunc = std::equal_to<T> >
inline Iterator
find_in_first(const T&        p_first,
              Iterator        p_first_it,
              Iterator        p_last_it,
              const CompFunc  p_comparator = CompFunc())
{
  return 
    std::find_if(p_first_it, p_last_it,
                 [&](auto& v){
                   return p_comparator(v.first, p_first);
                 });
}

template<typename     T,
         typename     Iterator,
         typename     CompFunc = std::equal_to<T> >
inline Iterator
find_in_second(const T&        p_second,
               Iterator        p_first_it,
               Iterator        p_last_it,
               const CompFunc  p_comparator = CompFunc())
{
  return 
    std::find_if(p_first_it, p_last_it,
                 [&](auto& v){
                   return p_comparator(v.second, p_second);
                 });
}

template<template<class> class OptionalType,
//         typename     T,
         typename     Iterator,
         typename     F = typename std::iterator_traits<Iterator>::value_type::first_type,
         typename     S = typename std::iterator_traits<Iterator>::value_type::second_type,
         typename     CompFunc = std::equal_to<F> >
inline OptionalType<S>
to_second(const F&        p_first,
          Iterator        p_first_it,
          Iterator        p_last_it,
          const CompFunc  p_comparator = CompFunc())
{
  OptionalType<S>   result;
  
  auto it = find_in_first(p_first, p_first_it, p_last_it, p_comparator);

  if (it != p_last_it)
  {
    result = it->second;
  }

  return result;
}

template<template<class> class OptionalType,
         typename     F,
         typename     S,
         std::size_t  N,
         typename     CompFunc = std::equal_to<F> >
inline OptionalType<S>
to_second(const F&   p_first,
          const std::pair<F, S> (&p_map)[N],
          const CompFunc  p_comparator = CompFunc())
{
  return to_second<OptionalType>(p_first, 
                                 std::begin(p_map), 
                                 std::end(p_map),
                                 p_comparator);
}

template<template<class> class OptionalType,
         typename     Iterator,
         typename     F = typename std::iterator_traits<Iterator>::value_type::first_type,
         typename     S = typename std::iterator_traits<Iterator>::value_type::second_type,
         typename     CompFunc = std::equal_to<S> >
inline OptionalType<F>
to_first(const S&         p_second,
         Iterator         p_first_it,
         Iterator         p_last_it,
         const CompFunc   p_comparator = CompFunc())
{
  OptionalType<F>   result;

  auto it = find_in_second(p_second, p_first_it, p_last_it, p_comparator);

  if (it != p_last_it)
  {
    result = it->first;
  }
  
  return result;
}

template<template<class> class OptionalType,
         typename     F,
         typename     S,
         std::size_t  N,
         typename     CompFunc = std::equal_to<S> >
inline OptionalType<F>
to_first(const S&   p_second,
         const std::pair<F, S>  (&p_map)[N],
         const CompFunc  p_comparator = CompFunc())
{
  return to_first<OptionalType>(p_second, 
                                std::begin(p_map), 
                                std::end(p_map));
}

template<template<class> class OptionalType,
         typename     F,
         typename     S,
         template<class, class> class C,
         typename     CompFunc = std::equal_to<S> >
inline OptionalType<F>
to_first(const S                  &p_second,
         const C<std::pair<F,S>, std::allocator<std::pair<F,S>>>  &p_container,
         const CompFunc   p_comparator = CompFunc())
{
  return to_first<OptionalType>(p_second, 
                                std::begin(p_container), 
                                std::end(p_container));
}

} // namespace haluj

#endif // HALUJ_BIDIRECTIONAL_MAP_HPP
