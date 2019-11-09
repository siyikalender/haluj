/// \file bitops.hpp
/// A Ring Buffer implementation
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

#ifndef HALUJ_BASE_RING_BUFFER_HPP
#define HALUJ_BASE_RING_BUFFER_HPP

#include <cstdint>
#include <algorithm>

#include "bitops.hpp"
#include "optional.hpp"
#include "cyclic_index.hpp"
#include "null_lock.hpp"

namespace haluj
{

namespace base
{

template< typename      RandomAccessContainerType, 
          typename      FlagsType  = uint8_t,
          typename      ScopedLock = null_lock>
struct ring_buffer
{
  typedef RandomAccessContainerType             container_type;
  typedef typename container_type::value_type   base_type;
  typedef base_type*                            iterator;
  typedef std::size_t                           index_type;
  typedef FlagsType                             flags_type;


  static constexpr uint8_t c_empty = 0U;
  static constexpr uint8_t c_full  = 1U;

  ring_buffer()
  {
    clear(true);
  }

  void clear(const bool p_zero = false)
  {
    m_flags   = mask(c_empty); // non atomic due to store
    m_tail    = 0;
    m_head    = 0;
  }

  constexpr std::size_t capacity() const
  {
    return m_container.size();
  }
  
  /// size: how many elements are written/available to read
  std::size_t size() const
  {
    std::size_t result;
    
    if (empty())
      result = 0;
    else if (full())
      result = capacity();
    else
    {
      if (m_head < m_tail)
      {
        result = capacity() + m_head - m_tail;
      }
      else
      {
        result = m_head - m_tail;
      }
    }
    return result;
  }

  /// remaining: remaining free space to write
  std::size_t remaining() const
  {
    return capacity() - size();
  }  

  bool full() const
  {
    return bit_test(uint8_t(m_flags), c_full); 
  }

  bool empty() const
  {
    return bit_test(uint8_t(m_flags), c_empty);
  }
  
  void push(const base_type &p_data)
  {
    ScopedLock lock;
    m_container[m_head] = p_data;
    m_head      =   cyclic_increment(m_head, capacity());
    m_flags     &=  ~mask(c_empty); // atomic, depending on flags_type
    if (m_head == m_tail)
    {
      m_flags |= mask(c_full);      // atomic, depending on flags_type
    }
  }

  void pop()
  {
    ScopedLock lock;
    m_tail       = cyclic_increment(m_tail, capacity());
    m_flags     &= ~mask(c_full); // atomic, depending on flags_type
    if (m_head == m_tail)
    {
      m_flags |= mask(c_empty);   // atomic, depending on flags_type
    }
  }
  
  base_type& front() 
  {
    return m_container[m_tail];
  }

  container_type      m_container;
  index_type          m_tail;
  index_type          m_head;
  flags_type          m_flags; // <--- should be atomic
};

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_RING_BUFFER_HPP
