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

#ifndef HALUJ_BASE_DIGITAL_INPUT_FILTER_HPP
#define HALUJ_BASE_DIGITAL_INPUT_FILTER_HPP

#include <algorithm>

template<unsigned BufferSize>
struct digital_input_filter
{
  static constexpr unsigned c_buffer_size = BufferSize;

  digital_input_filter(const uint32_t p_mask = 0U)
  : m_mask(p_mask),
    m_value(0U),
    m_edge(0U)
  {
    std::fill_n(&m_input[0], c_buffer_size, 0U);
  }

  uint32_t operator()(const uint32_t      p_input)
  {
    for (unsigned i = (c_buffer_size - 1U); i > 0U; i--)
    {
      m_input[i]  =   m_input[i - 1U];
    }

    uint32_t result = m_input[0U] = p_input & m_mask;

    for (unsigned i = 1U; i < c_buffer_size; i++)
    {
      result      &=  m_input[i];
    }

    m_edge              = result ^ m_value;

    if (m_edge != 0)
    {
      m_value             = result;
    }
    
    return m_value;
  }

  uint32_t      value() const
  {
    return m_value;
  }

  // be aware m_edge changes in c_time_step.
  uint32_t      edge() const
  {
    return m_edge;
  }
  
  uint32_t mask() const
  {
    return m_mask;
  }

  uint32_t      m_input[c_buffer_size];
  uint32_t      m_mask;
  uint32_t      m_value;
  uint32_t      m_edge;
};

// HALUJ_BASE_DIGITAL_INPUT_FILTER_HPP
#endif
