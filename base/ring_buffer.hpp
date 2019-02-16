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

#include "optional.hpp"
#include "cyclic_index.hpp"
#include "null_lock.hpp"

namespace haluj
{

namespace base
{

template< typename      BaseType, 
          typename      ScopedLock = null_lock>
struct ring_buffer_adaptor
{
  typedef BaseType                        base_type;
  typedef BaseType*                       iterator;
  typedef std::size_t                     index_type;

  ring_buffer_adaptor(iterator const p_first, 
                      iterator const p_last)
  : m_data_begin(p_first),
    m_data_end(p_last)
  {
    clear(true);
  }

  void clear(const bool p_zero = false)
  {
    m_full    = false;
    m_empty   = true;
    m_tail    = 0;
    m_head    = 0;
    if (p_zero)
      std::fill(m_data_begin, m_data_end, BaseType());
  }

  constexpr std::size_t size() const
  {
    return (m_data_end - m_data_begin);
  }
  
  /// available: how many elements are written/available to read
  std::size_t available() const
  {
    std::size_t result;
    
    if (m_empty)
      result = 0;
    else if (m_full)
      result = size();
    else
    {
      if (m_head < m_tail)
      {
        result = size() + m_head - m_tail;
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
    return size() - available();
  }  


  bool full() const
  {
    return m_full; // hata var cyclic olması göz önüne alınmalı
  }

  bool empty() const
  {
    return m_empty;
  }
  
  optional<BaseType> read()
  {
    optional<BaseType> result;
    {
      ScopedLock lock;
      if (!empty())
      {
        result      = m_data_begin[m_tail];
        m_tail      = cyclic_increment(m_tail, size());
        m_empty     = (m_head == m_tail);
        m_full      = false;
      }
    }
    return result;
  }

  optional<BaseType> peek() const
  {
    optional<BaseType> result;
    {
      ScopedLock lock;

      if (!empty())
      {
        result = m_data_begin[m_tail];
      }
    }
    return result;
  }

  bool write(const BaseType &p_data)
  {
    bool          result = false;
    {
      ScopedLock lock;

      if (!full())
      {
        m_data_begin[m_head] = p_data;
        m_head      = cyclic_increment(m_head, size());
        m_empty     = false;
        m_full      = (m_head == m_tail);
        result      = true;
      }
    }
    return result;
  }

  template<std::size_t  N>
  bool set(base_type (&p_data)[N])
  {
    // static_assert(N <= size(), "ring_buffer::set : input size exceeds limits");
    bool result = false;
    
    if (N <= size())
    {
      std::copy_n(&p_data[0], N, &m_data_begin[0]);
      m_tail = 0;
      if (N < size())
        m_head = N;
      else
        m_head = 0;
      m_full  = (N == size());
      m_empty = false;
      result  = true;
    }

    return result;
  }

  iterator    const m_data_begin;
  iterator    const m_data_end;
  index_type  m_tail;
  index_type  m_head;
  bool        m_full;
  bool        m_empty;
};

template< typename      BaseType, 
          std::size_t   Size,
          typename      ScopedLock = null_lock>
struct ring_buffer
: ring_buffer_adaptor<BaseType, ScopedLock>
{
  ring_buffer()
  : ring_buffer_adaptor<BaseType, ScopedLock>(&m_data[0], &m_data[Size])
  {}
  
  BaseType   m_data[Size];
};

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_RING_BUFFER_HPP
