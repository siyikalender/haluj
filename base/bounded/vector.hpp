/**
 ** This file is released under Creative Commons Zero - CC0
 **/

#ifndef HALUJ_BASE_BOUNDED_VECTOR_HPP
#define HALUJ_BASE_BOUNDED_VECTOR_HPP

// #include "array_size.hpp"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <iterator>
#include <algorithm>

namespace haluj
{
  
namespace base
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

  template <typename Iterator>
  void assign(Iterator first, Iterator last)
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

template<typename T, std::size_t Capacity>
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

template<typename T, std::size_t Capacity>
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

} // base

} // haluj

#endif // HALUJ_BASE_BOUNDED_VECTOR_HPP
