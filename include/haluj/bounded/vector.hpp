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

#include <algorithm>
#include <cstddef>
#include <iterator>

namespace haluj
{
  
namespace bounded
{

/// A vector with its storage inside the object: at most Capacity elements,
/// no dynamic allocation.
///
/// The member functions do not check the capacity or the size. The caller
/// checks first:
///   - push_back() requires !full()
///   - pop_back(), front() and back() require !empty()
///   - resize(n) requires n <= capacity()
///   - assign(first, last) requires distance(first, last) <= capacity()
///   - operator[](i) requires i < size()
/// The free functions push_back, pop_back, resize and assign below are the
/// checked versions: they return false and leave the vector unchanged when
/// the call does not fit.
///
/// The storage holds Capacity elements from the start, so T must be default
/// constructible, and an element that leaves the vector (pop_back, resize,
/// clear) is not destroyed until it is overwritten. Both are natural for the
/// trivial types this vector is meant for.
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

  // Copying and assigning are left to the compiler (rule of zero): they copy
  // the whole storage, so a vector of trivially copyable T is trivially
  // copyable. The user provided default constructor keeps the type from
  // being an aggregate, so that vector<int, 4> v{1, 2} does not compile
  // into a vector of size 0 with 1 and 2 in its storage.
  vector()
  {}

  /// \pre !full()
  void push_back(const T& p_value)
  {
    m_data[m_size] = p_value;
    m_size++;
  }

  /// \pre !empty()
  void pop_back()
  {
    m_size--;
  }

  /// \pre p_size <= capacity()
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

  /// Takes a forward range: it is measured, then copied.
  /// \pre distance(first, last) <= capacity()
  template <typename ForwardIterator>
  void assign(ForwardIterator first, ForwardIterator last)
  {
    resize(std::distance(first, last));
    std::copy (first, last, begin());
  }

  bool empty() const
  {
    return size() == 0;
  }

  bool full() const
  {
    return size() == Capacity;
  }

  iterator begin()
  {
    return m_data;
  }

  // m_data + m_size rather than &m_data[m_size], which would index past the
  // array when the vector is full
  iterator end()
  {
    return m_data + m_size;
  }

  const_iterator begin() const
  {
    return m_data;
  }

  const_iterator end() const
  {
    return m_data + m_size;
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
    return const_reverse_iterator(end());
  }

  const_reverse_iterator rend() const
  {
    return const_reverse_iterator(begin());
  }

  reference front()
  {
    return *begin();
  }

  reference back()
  {
    return m_data[m_size - 1];
  }

  const_reference front() const
  {
    return *begin();
  }

  const_reference back() const
  {
    return m_data[m_size - 1];
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

// Checked versions of the member functions

/// Appends v if there is room
template<typename     T,
         std::size_t  Capacity>
inline bool push_back(vector<T, Capacity>& c, const T& v)
{
  bool result = false;
  
  if (c.size() < c.capacity())
  {
    c.push_back(v);
    result = true;
  }
  
  return result;
}

/// Removes the last element if there is one
template<typename     T,
         std::size_t  Capacity>
inline bool pop_back(vector<T, Capacity>& c)
{
  bool result = false;

  if (!c.empty())
  {
    c.pop_back();
    result = true;
  }

  return result;
}

/// Replaces the contents with a forward range if it fits
template<typename     T,
         std::size_t  Capacity,
         typename     ForwardIterator>
inline bool assign(vector<T, Capacity>& p_c,
                   ForwardIterator      p_first,
                   ForwardIterator      p_last)
{
  bool result = false;

  const auto d = std::distance(p_first, p_last);

  if (d >= 0 && static_cast<std::size_t>(d) <= p_c.capacity())
  {
    p_c.assign(p_first, p_last);
    result = true;
  }

  return result;
}

/// Changes the size if it fits; new elements are copies of p_initial
template<typename     T,
         std::size_t  Capacity>
inline bool resize(vector<T, Capacity>&  c,
                   const std::size_t     p_size,
                   const T&              p_initial = T())
{
  bool result = false;

  if (p_size <= Capacity)
  {
    c.resize(p_size, p_initial);
    result = true;
  }

  return result;
}

} // bounded

} // haluj

#endif // HALUJ_BOUNDED_VECTOR_HPP
