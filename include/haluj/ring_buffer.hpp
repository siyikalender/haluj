/// \file ring_buffer.hpp
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

#ifndef HALUJ_RING_BUFFER_HPP
#define HALUJ_RING_BUFFER_HPP

#include <cstdint>
#include <algorithm>

#include "bit/field.hpp"
#include "bit/pack.hpp"
#include "bit/storage.hpp"
#include "cyclic_index.hpp"

namespace haluj
{
/// A fixed capacity ring buffer (FIFO) over a random access container.
///
/// The element functions do not check the state of the buffer. The caller
/// checks it first:
///   - push() requires !full()   (or remaining() > 0)
///   - pop(), front() and back() require !empty()   (or size() > 0)
/// Calling them otherwise leaves the buffer in an undefined state.
///
/// \code {.cpp}
/// haluj::ring_buffer<std::array<char, 64>> rx;
///
/// if (!rx.full())
///   rx.push(c);
///
/// while (!rx.empty())
/// {
///   process(rx.front());
///   rx.pop();
/// }
/// \endcode

template< typename  RandomAccessContainerType, 
          typename  FlagsBaseType  = uint8_t>
struct ring_buffer
{
  typedef RandomAccessContainerType             container_type;
  typedef typename container_type::value_type   base_type;
  typedef base_type*                            iterator;
  typedef std::size_t                           index_type;
  typedef FlagsBaseType                         flags_base_type;

  struct empty_bit  : bit::field<0> {};
  struct full_bit   : bit::field<1> {};
  
  using flags_type =
    bit::storage
    <
      bit::pack
      <
        flags_base_type, 
        empty_bit, 
        full_bit
      >
    >;

  ring_buffer()
  {
    clear();
  }

  /// clear the buffer
  void clear()
  {
    m_flags.template set<empty_bit>();
    m_flags.template clear<full_bit>();
    m_tail    = 0;
    m_head    = 0;
  }

  /// capacity: maximum number of elements the buffer holds
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

  /// full: returns true if buffer is full
  bool full() const
  {
    return m_flags.template test<full_bit>(); 
  }

  /// empty: returns true if buffer is empty
  bool empty() const
  {
    return m_flags.template test<empty_bit>(); 
  }
  
  /// push: Add a new element to head
  /// \pre !full()
  void push(const base_type &p_data)
  {
    m_container[m_head] = p_data;
    m_head      =   cyclic_increment(m_head, capacity());
    m_flags.template clear<empty_bit>();
    if (m_head == m_tail)
    {
      m_flags.template set<full_bit>();
    }
  }

  /// pop: remove element from the tail
  /// \pre !empty()
  void pop()
  {
    m_tail       = cyclic_increment(m_tail, capacity());
    m_flags.template clear<full_bit>();
    if (m_head == m_tail)
    {
      m_flags.template set<empty_bit>();
    }
  }
  
  /// front: oldest element, the next one pop() removes
  /// \pre !empty()
  base_type& front() 
  {
    return m_container[m_tail];
  }

  /// back: newest element, the last one push() added
  /// \pre !empty()
  base_type& back() 
  {
    return m_container[m_head - 1];
  }

  container_type      m_container;
  index_type          m_tail;
  index_type          m_head;
  flags_type          m_flags;
};

} // namespace haluj

#endif // HALUJ_RING_BUFFER_HPP
