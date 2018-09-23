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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_UART_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_UART_HPP

#include "peripheral.hpp"
#include "options.hpp"
#include "core.hpp"
#include "bitops.hpp"

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
  static constexpr UART_Type*  uart_addr()       {return Specifier::uart_addr;}
  static constexpr bool        uses_core_clock() {return Specifier::uses_core_clock;}
};

/// \fn set_baud_rate
/// \brief Sets baud rate registers. Assumed UART BDH register is cleared
inline uint32_t 
set_baud_rate(UART_Type&      p_device, 
              const uint32_t  p_peripheral_clock, 
              const uint32_t  p_baud_rate) 
{
  uint32_t result = (p_peripheral_clock / (p_baud_rate * 16U));;
  p_device.BDH |= UART_BDH_SBR(result >> 8); 
  p_device.BDL  = UART_BDL_SBR(result);
  return result;
}

/// \fn set_fractional_divider
/// \brief Sets fractional divider. Assumed UART C4 register is cleared
inline uint32_t 
set_fractional_divider(UART_Type&      p_device, 
                       const uint32_t  p_peripheral_clock, 
                       const uint32_t  p_baud_rate,
                       const uint32_t  p_sbr) 
{
  uint32_t  result  = ((p_peripheral_clock * 2)/(p_baud_rate)) - (p_sbr * 32);
  p_device.C4 |= UART_C4_BRFA(result);
  return result;
}

/// \fn set_bits
/// \brief Sets data bits. Assumed UART C1 and C4 register is cleared
inline void
set_bits(UART_Type&       p_device, 
         const uint32_t   p_value)
{
  switch(p_value)
  {
  default:
    break;
  case 9:
    p_device.C1 = mask_set(p_device.C1, UART_C1_M_MASK);
    break;
  case 10:
    p_device.C4 = mask_set(p_device.C4, UART_C4_M10_MASK);
    break;
  }
}

/// \fn set_parity
/// \brief Sets parity. Assumed UART C1 is cleared
inline void
set_parity(UART_Type&       p_device, 
           const uint32_t   p_value)
{
  // UARTx_C1 register is assumed to be cleared
  switch(p_value)
  {
  default: // 0 none
    break;
  case 1:
    p_device.C1 = mask_set(p_device.C1, UART_C1_PE_MASK);
    break;
  case 2:
    p_device.C1 = mask_set(p_device.C1, UART_C1_PE_MASK | UART_C1_PT_MASK);
    break;
  }
}

/// \fn set_stop_bits
/// \brief Sets stop bits. Assumed UART BDH SBNS field is cleared
inline void
set_stop_bits(UART_Type&       p_device, 
              const uint32_t   p_value)
{
  // UARTx_C1 register is assumed to be cleared
  switch(p_value)
  {
  default:  // one
    break;
  case 1:   // two
    #ifdef UART_BDH_SBNS_MASK
    p_device.BDH = mask_set(p_device.BDH, UART_BDH_SBNS_MASK);
    #endif
    break;
  }
}

template<uint32_t Value> 
struct baud_rate
{
  static constexpr uint32_t value    = Value;
  
  template<typename T>
  uint32_t accept(uart<T>)
  {
    uint32_t peripheral_clock = system_core_clock();
    /*static*/ if (!uart<T>::uses_core_clock())
    {
      peripheral_clock = system_bus_clock();
    }
    uint32_t sbr = set_baud_rate(*uart<T>::uart_addr(),  peripheral_clock, value);
    (void) set_fractional_divider(*uart<T>::uart_addr(), peripheral_clock, value, sbr);
    return  0;
  }
};

template<uint32_t Value> 
struct bits_option
{
  static constexpr uint32_t value    = Value;
  
  template<typename T>
  uint32_t accept(uart<T>)
  {
    set_bits(*uart<T>::uart_addr(), value);
    return  0;
  }
};

struct bits
{
  static bits_option<8>   _8;
  static bits_option<9>   _9;
  static bits_option<10>  _10;
};

template<uint32_t Value> 
struct parity_option
{
  static constexpr uint32_t value    = Value;
  
  template<typename T>
  uint32_t accept(uart<T>)
  {
    set_parity(*uart<T>::uart_addr(), value);
    return  0;
  }
};

struct parity
{
  static parity_option<0>   none;
  static parity_option<1>   even;
  static parity_option<2>   odd ;
};

template<uint32_t Value> 
struct stop_bits_option
{
  static constexpr uint32_t value    = Value;
  
  template<typename T>
  uint32_t accept(uart<T>)
  {
    set_stop_bits(*uart<T>::uart_addr(), value);
    return  0;
  }
};

struct stop_bits
{
  static stop_bits_option<0>    one;
  static stop_bits_option<1>    two;
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

template<typename T, typename Options>
inline void configure(uart<T> p_uart, Options p_opts)
{
  stop(p_uart);
  uart<T>::uart_addr()->C1  = 0;
  uart<T>::uart_addr()->C4  = 0;
  uart<T>::uart_addr()->BDH = 0;
  p_opts.template accept<uint32_t>(p_uart, null_op());
  start(p_uart);
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj


// HALUJ_BASE_DEVICES_ARM_KINETIS_UART_HPP
#endif

