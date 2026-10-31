/// \file digital_input_filter.hpp
/// software based dijital input filter
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

#ifndef HALUJ_DIGITAL_INPUT_FILTER_HPP
#define HALUJ_DIGITAL_INPUT_FILTER_HPP

#include <algorithm>

namespace haluj
{

// Debounce policies
//
// The filter keeps the last BufferSize samples of each input bit. After
// every sample it works out two masks:
//   ones   the bits that were 1 in every sample
//   zeros  the bits that were 0 in every sample
// and the policy turns them, with the current value, into the new value.

/// A 0 to 1 change needs BufferSize equal samples, a 1 to 0 change is
/// immediate. For active high inputs whose activation must be confirmed.
struct debounce_rising
{
  template<typename T>
  static T apply(T, T ones, T, T)
  {
    return ones;
  }
};

/// A 1 to 0 change needs BufferSize equal samples, a 0 to 1 change is
/// immediate. The mirror of debounce_rising, for active low inputs.
struct debounce_falling
{
  template<typename T>
  static T apply(T, T, T zeros, T mask)
  {
    return T(~zeros & mask);
  }
};

/// Both changes need BufferSize equal samples; while the samples disagree a
/// bit keeps its value. A single noisy sample changes nothing.
struct debounce_both
{
  template<typename T>
  static T apply(T value, T ones, T zeros, T)
  {
    return T((value | ones) & ~zeros);
  }
};

/// Filters up to the width of T digital inputs, one per bit, sampled by
/// calling operator() with the raw input. Bits outside the mask are ignored
/// and read as 0.
template<typename T, unsigned BufferSize, typename Debounce = debounce_rising>
struct digital_input_filter
{
  static_assert(BufferSize > 0U, "digital_input_filter needs at least one sample");

  static constexpr unsigned c_buffer_size = BufferSize;

  digital_input_filter(const T p_mask = 0U)
  : m_mask(p_mask),
    m_value(0U),
    m_edge(0U)
  {
    std::fill_n(&m_input[0], c_buffer_size, 0U);
  }

  T operator()(const T      p_input)
  {
    for (unsigned i = (c_buffer_size - 1U); i > 0U; i--)
    {
      m_input[i]  =   m_input[i - 1U];
    }

    m_input[0U] = T(p_input & m_mask);

    T ones  = m_input[0U];
    T zeros = T(~m_input[0U]);

    for (unsigned i = 1U; i < c_buffer_size; i++)
    {
      ones  = T(ones  &  m_input[i]);
      zeros = T(zeros & ~m_input[i]);
    }

    zeros = T(zeros & m_mask);

    const T result = Debounce::apply(m_value, ones, zeros, m_mask);

    m_edge  = T(result ^ m_value);
    m_value = result;

    return m_value;
  }

  T      value() const
  {
    return m_value;
  }

  T      edge() const
  {
    return m_edge;
  }
  
  T mask() const
  {
    return m_mask;
  }

  T      m_input[c_buffer_size];
  T      m_mask;
  T      m_value;
  T      m_edge;
};

} // namespace haluj

// HALUJ_DIGITAL_INPUT_FILTER_HPP
#endif
