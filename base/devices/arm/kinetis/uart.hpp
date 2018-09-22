/// \file port.hpp
/// generic port definitions for Kinetis family devices
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

#ifndef _HALUJ_BASE_DEVICES_KINETIS_UART_HPP
#define _HALUJ_BASE_DEVICES_KINETIS_UART_HPP

#include "peripheral.hpp"

namespace haluj
{

namespace base
{

namespace devices
{

namespace arm
{
  
namespace kinetis
{

template <typename Specifier>
struct uart : peripheral<Specifier>
{
  static constexpr UART_Type*  uart_addr() {return Specifier::uart_addr;}
}; 

struct bits
{
  static constexpr uint32_t   _8  = 0;
  static constexpr uint32_t   _9  = UART_C1_M_MASK;
};

struct parity
{
  static constexpr uint32_t   none  = 0U;
  static constexpr uint32_t   even  = 2U;
  static constexpr uint32_t   odd   = 3U;
};

struct stop_bits
{
  static constexpr uint32_t   one   = 0U;
  #ifdef UART_BDH_SBNS_MASK
  static constexpr uint32_t   two   = UART_BDH_SBNS_MASK;
  #endif
};

template<typename T>
inline void start(uart<T>)
{
  uart<T>::uart_addr()->C2 = 
    mask_set(uart<T>::uart_addr()->C2, 
             UART_C2_TE_MASK | UART_C2_RE_MASK);
}

template<typename T>
inline void stop(uart<T>)
{
  uart<T>::uart_addr()->C2 = 
    mask_clear(uart<T>::uart_addr()->C2, 
               UART_C2_TE_MASK | UART_C2_RE_MASK);
}

extern void configure_uart(UART_Type&       p_device_const,
                           const uint32_t   p_peripheral_clock,
                           const uint32_t   p_baud_rate,
                           const uint32_t   p_bit_mode,
                           const uint32_t   p_parity,
                           const uint32_t   p_stop_bits);

template<typename T>
inline void configure(uart<T>, 
                      const uint32_t    p_baud_rate = 9600,
                      const uint32_t    p_bit_mode  = bits::_8,
                      const uint32_t    p_parity    = parity::none,
                      const uint32_t    p_stop_bits = stop_bits::one)
{
  stop(uart<T>());
  configure_uart(*uart<T>::uart_addr(), 
                 SystemCoreClock, 
                 p_baud_rate, 
                 p_bit_mode,
                 p_parity, 
                 p_stop_bits);
  start(uart<T>());
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj


// _HALUJ_BASE_DEVICES_KINETIS_UART_HPP
#endif

