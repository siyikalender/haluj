/// \file optional.hpp
/// A lightweight std optional implementation
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

#ifndef HALUJ_OPTIONAL_HPP
#define HALUJ_OPTIONAL_HPP

#include <utility>

namespace haluj
{

struct nullopt_t
{
  explicit constexpr nullopt_t(int) {}
};

constexpr nullopt_t nullopt = nullopt_t(0);

template<typename T>
struct optional
{
  optional()
  : m_initialized(false), 
    m_value()
  {}
  
  optional(const optional& p_other)
  : m_initialized(p_other.m_initialized), 
    m_value(p_other.m_value)
  {}
  
  optional(const T& p_value)
  : m_initialized(true), 
    m_value(p_value)
  {}
  
  explicit operator bool() const
  {
    return has_value();
  }
  
  bool has_value() const
  {
    return m_initialized;
  }

  void reset() 
  {
    m_initialized = false;
  }
  
  T& operator *() &
  {
    return m_value;
  }

  const T& operator *() const&
  {
    return m_value;
  }

  template<typename U>
  optional& operator=(U&& p_value)
  {
    m_initialized = true;
    m_value       = std::forward<U>(p_value);
    return *this;
  }
  
  optional& operator=(const optional& p_other)
  {
    m_initialized = p_other.m_initialized;
    m_value       = p_other.m_value;
    return *this;
  }
  
  optional& operator=(nullopt_t)
  {
    reset();
    return *this;
  }

  bool  m_initialized;
  T     m_value;
};

} // namespace haluj

#endif // HALUJ_OPTIONAL_HPP
