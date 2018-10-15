/// \file pin.hpp
/// generic pin range implementation
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

#ifndef HALUJ_BASE_PIN_RANGE_HPP
#define HALUJ_BASE_PIN_RANGE_HPP

namespace haluj
{

namespace base
{

template<typename Port, uint32_t Start, uint32_t End >
struct pin_range
{
  typedef Port port;
  
  static constexpr uint32_t start   = Start;
  static constexpr uint32_t end     = End;

  static constexpr uint32_t shift()
  {
    return start;
  }
  
  static uint32_t mask()
  {
    return mask_range(start, end);
  }

  static void set()
  {
    port::set(mask());
  }

  static void clear()
  {
    port::clear(mask());
  }

  static void toggle()
  {
    port::toggle(mask());
  }
  
  static uint32_t read()
  {
    return (port::read() & mask()) >> shift();
  }  
  
  static void write(const uint32_t p_value)
  {
    uint32_t tmp = port::read_out(); // read current port value
    const uint32_t m = mask();
    tmp &= ~m;
    uint32_t u_val = p_value;
    u_val <<= shift();
    u_val &= m; // clip values out of boundary
    tmp |= u_val;
    port::write(tmp);
  }
  
  static void set_direction()
  {
    port::set_direction(mask());
  }

  static void clear_direction()
  {
    port::clear_direction(mask());
  }  

};

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_PIN_RANGE_HPP
