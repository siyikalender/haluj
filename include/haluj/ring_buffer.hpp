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

#include <cstddef>

#include "concurrency.hpp"
#include "cyclic_index.hpp"

namespace haluj
{

/// A fixed capacity ring buffer (FIFO) over a random access container, such
/// as std::array<char, 64>, whose size is the capacity. Any capacity works.
///
/// Positions: the head (where the next element goes) and the tail (the
/// oldest element) count round 2 * capacity, not capacity. With positions
/// round capacity, an empty and a full buffer both have head == tail; round
/// 2 * capacity they differ: empty is head == tail, full is
/// head - tail == capacity (mod 2 * capacity). The element of a position p
/// is at p, or at p - capacity when p >= capacity.
///
/// The element functions do not check the state of the buffer. The caller
/// checks it first:
///   - push() requires !full()   (or remaining() > 0)
///   - pop(), front() and back() require !empty()   (or size() > 0)
/// Calling them otherwise leaves the buffer in an undefined state.
/// try_push() and try_pop() check by themselves and return false instead.
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
///
/// Concurrency: single_context (the default) or spsc. With spsc the producer
/// calls only push, try_push, full and remaining, and the consumer only pop,
/// try_pop, front and empty; size() may be called by either and gives a
/// value that was true at some moment of the call. clear() and back() are
/// for when the other side is not running: back() reads the newest element,
/// which in spsc use the producer may be writing at that moment.
///
/// A ring_buffer is built at compile time: as a global, declare it constinit
/// and it starts in .bss, with no code run at start up. The container is
/// value initialized, so a local ring_buffer clears its storage when built.
template< typename  RandomAccessContainerType,
          typename  Concurrency = single_context>
struct ring_buffer
{
  typedef RandomAccessContainerType             container_type;
  typedef typename container_type::value_type   base_type;
  typedef base_type                             value_type;
  typedef std::size_t                           index_type;
  typedef Concurrency                           concurrency;

  // Declared so the type is not an aggregate: ring_buffer{...} with values
  // would fill the container and the positions directly
  constexpr ring_buffer() = default;

  /// clear: empty the buffer
  constexpr void clear()
  {
    m_tail.store(0);
    m_head.store(0);
  }

  /// capacity: maximum number of elements the buffer holds
  constexpr std::size_t capacity() const
  {
    return m_container.size();
  }

  /// size: how many elements are written/available to read
  constexpr std::size_t size() const
  {
    const index_type tail = m_tail.load();
    const index_type head = m_head.load();
    return cyclic_distance_forward(tail, head, modulus());
  }

  /// remaining: remaining free space to write
  constexpr std::size_t remaining() const
  {
    return capacity() - size();
  }

  /// full: returns true if buffer is full
  constexpr bool full() const
  {
    return size() == capacity();
  }

  /// empty: returns true if buffer is empty
  constexpr bool empty() const
  {
    return m_head.load() == m_tail.load();
  }

  /// push: Add a new element to head
  /// \pre !full()
  constexpr void push(const base_type &p_data)
  {
    const index_type head = m_head.load();
    m_container[element(head)] = p_data;
    m_head.store(cyclic_increment(head, modulus()));
  }

  /// pop: remove element from the tail
  /// \pre !empty()
  constexpr void pop()
  {
    m_tail.store(cyclic_increment(m_tail.load(), modulus()));
  }

  /// try_push: push if there is room
  constexpr bool try_push(const base_type &p_data)
  {
    bool result = false;

    if (!full())
    {
      push(p_data);
      result = true;
    }

    return result;
  }

  /// try_pop: take the oldest element into p_data if there is one
  constexpr bool try_pop(base_type &p_data)
  {
    bool result = false;

    if (!empty())
    {
      p_data = front();
      pop();
      result = true;
    }

    return result;
  }

  /// front: oldest element, the next one pop() removes
  /// \pre !empty()
  constexpr base_type& front()
  {
    return m_container[element(m_tail.load())];
  }

  constexpr const base_type& front() const
  {
    return m_container[element(m_tail.load())];
  }

  /// back: newest element, the last one push() added
  /// \pre !empty()
  /// In spsc use, call it only while the producer is not running.
  constexpr base_type& back()
  {
    return m_container[element(cyclic_decrement(m_head.load(), modulus()))];
  }

  constexpr const base_type& back() const
  {
    return m_container[element(cyclic_decrement(m_head.load(), modulus()))];
  }

  typedef shared<index_type, concurrency> position_type;

  container_type      m_container{};
  position_type       m_tail;
  position_type       m_head;

private:

  // The positions count round twice the capacity
  constexpr index_type modulus() const
  {
    return 2U * capacity();
  }

  // Where in the container the element of a position is
  constexpr index_type element(const index_type p_position) const
  {
    return (p_position < capacity()) ? p_position : (p_position - capacity());
  }
};

} // namespace haluj

#endif // HALUJ_RING_BUFFER_HPP
