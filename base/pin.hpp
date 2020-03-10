/// \file pin.hpp
/// generic pin implementation
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

#ifndef HALUJ_BASE_PIN_HPP
#define HALUJ_BASE_PIN_HPP

#include <cstdint>

#include "bitops.hpp"

namespace haluj
{

namespace base
{

template<typename Port, uint32_t Index>
struct pin
{

  typedef Port port;
  
  static constexpr uint32_t index  = Index;

  static void set()
  {
    port::set(mask(index));
  }

  static void clear()
  {
    port::clear(mask(index));
  }

  static void toggle()
  {
    port::toggle(mask(index));
  }
  
  static bool test()
  {
    return bit_test(port::read(), index);
  }
  
  static bool test_out()
  {
    return bit_test(port::read_out(), index);
  }

  static void set_direction()
  {
    port::set_direction(mask(index));
  }

  static void clear_direction()
  {
    port::clear_direction(mask(index));
  }

  static bool test_interrupt_clear()
  {
    return port::test_interrupt_clear(index);
  }
  
  static void assign(bool p_value)
  {
    if (p_value) set(); else clear();
  }
  
  template<typename Options>
  static void configure(Options opts)
  {
    port::configure(index, opts);
  }

};

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_PIN_HPP
