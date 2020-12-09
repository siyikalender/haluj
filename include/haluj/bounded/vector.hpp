/// \file vector.hpp
/// STL like fixed capacity vector implementation
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

#ifndef HALUJ_BOUNDED_VECTOR_HPP
#define HALUJ_BOUNDED_VECTOR_HPP

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <iterator>
#include <algorithm>

namespace haluj
{
  
namespace bounded
{

template<typename T, std::size_t Capacity>
struct vector
{
  typedef T                     value_type;
  typedef T&                    reference;
  typedef const T&              const_reference;
  typedef T*                    pointer;
  typedef const T*              const_pointer;
  typedef pointer               iterator;
  typedef const_pointer         const_iterator;
  typedef std::size_t           size_type;
  typedef std::ptrdiff_t        difference_type;
  typedef std::reverse_iterator<iterator>        reverse_iterator;
  typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;

  vector()
  {}

  void push_back(const T& p_value)
  {
    m_data[m_size] = p_value;
    m_size++;
  }

  void resize(const std::size_t p_size, const T& p_initial = T())
  {
    if (m_size < p_size)
    {
      std::fill(begin() + m_size, begin() + p_size, p_initial);
    }
    m_size = p_size;
  }

  constexpr std::size_t capacity() const
  {
    return Capacity;
  }

  std::size_t size() const
  {
    return m_size;
  }

  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last)
  {
    resize(std::distance(first, last));
    std::copy (first, last, begin());
  }

  bool empty() const
  {
    return size() == 0;
  }

  iterator begin()
  {
    return iterator(std::addressof(m_data[0]));
  }

  iterator end()
  {
    return iterator(std::addressof(m_data[m_size]));
  }

  const_iterator begin() const
  {
    return const_iterator(std::addressof(m_data[0]));
  }

  const_iterator end() const
  {
    return const_iterator(std::addressof(m_data[m_size]));
  }

  reverse_iterator rbegin()
  {
    return reverse_iterator(end());
  }

  reverse_iterator rend()
  {
    return reverse_iterator(begin());
  }

  const_reverse_iterator rbegin() const
  {
    return reverse_iterator(end());
  }

  const_reverse_iterator rend() const
  {
    return reverse_iterator(begin());
  }

  reference front()
  {
    return *begin();
  }

  reference back()
  {
    return *end();
  }

  const_reference front() const
  {
    return *begin();
  }

  const_reference back() const
  {
    return *end();
  }

  void clear()
  {
    m_size = 0U;
  }

  reference operator[](size_type p_index)
  {
    return m_data[p_index];
  }

  const_reference operator[](size_type p_index) const
  {
    return m_data[p_index];
  }

  pointer data()
  {
    return &m_data[0];
  }

  const_pointer data() const
  {
    return &m_data[0];
  }

  T             m_data[Capacity];
  std::size_t   m_size = 0U;
};

template<typename     T, 
         std::size_t  Capacity>
inline bool push_back(vector<T, Capacity>& c, const T& v)
{
  bool result = false;
  
  if (c.size() <= c.capacity())
  {
    c.push_back(v);
    result = true;
  }
  
  return result;
}

template<typename     T, 
         std::size_t  Capacity, 
         typename     InputIterator>
inline bool assign(vector<T, Capacity>& p_c, 
                   InputIterator        p_first, 
                   InputIterator        p_last)
{
  bool result = false;
  
  if (p_c.size() < p_c.capacity())
  {
    p_c.assign(p_first, p_last);
    result = true;
  }
  
  return result;
}

template<typename     T, 
         std::size_t  Capacity>
bool resize(vector<T, Capacity>&  c,
            const std::size_t     p_size)
{
  bool result = false;

  if (p_size <= Capacity)
  {
    c.resize(p_size);
    result = true;
  }

  return result;
}

} // bounded

} // haluj

#endif // HALUJ_BOUNDED_VECTOR_HPP
