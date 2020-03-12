/// \file fragment.hpp
/// Fragments a large sized block to smaller chunks. 
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

#ifndef HALUJ_BASE_FRAGMENT_HPP
#define HALUJ_BASE_FRAGMENT_HPP

#include <cstdint>
#include <utility>

template<typename Iterator>
struct fragment
{
    
  typedef Iterator iterator;
    
  fragment(iterator p_first, 
           iterator p_last)
  : first_(p_first),
    last_(p_last)
  {}

  fragment(const fragment& p_other)
  : first_(p_other.first_),
    last_(p_other.last_)
  {}

  fragment& operator=(const fragment& p_other)
  {
    first_ = p_other.first_;
    last_  = p_other.last_;
    return *this;
  }

  ~fragment() {}

  operator bool () const
  {
    return (first_ != last_);
  }
  
  std::size_t size() const
  {
    std::distance(first_, last_);    
  }

  iterator begin() const
  {
    return first_;
  }

  iterator end() const
  {
    return last_;
  }

  iterator first_;
  iterator last_;
};


template<typename Container, typename Iterator = typename Container::const_iterator>
inline Iterator 
get_fragment_end_(const Container&   p_container,
                  Iterator           p_current,
                  const std::size_t  p_fragment_size)
{
  std::size_t remaining = std::distance(p_current, p_container.end());
  std::size_t f_size    = (remaining > p_fragment_size) ? p_fragment_size : remaining;
  return p_current + f_size;
}                                  

template<typename Container, typename Iterator = typename Container::const_iterator>
inline fragment<Iterator> 
make_fragment(const Container&  p_container,
              const std::size_t p_fragment_size)
{
  auto end = 
    get_fragment_end_(p_container, 
                      p_container.begin(), 
                      p_fragment_size) ;
  return fragment<Iterator>(p_container.begin(), end);
}


template<typename Container, typename Iterator = typename Container::const_iterator>
inline fragment<Iterator> 
make_fragment(const Container&            p_container,
              const std::size_t           p_fragment_size,
              const fragment<Iterator>&   p_f)
{
  auto end = 
    get_fragment_end_(p_container, 
                      p_f.end(), 
                      p_fragment_size) ;
  return fragment<Iterator>(p_f.end(), end);
}


#endif // HALUJ_BASE_FRAGMENT_HPP
