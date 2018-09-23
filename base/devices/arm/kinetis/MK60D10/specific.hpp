/// \file specific.hpp
/// specific definitions for Kinetis MK60D10 family devices
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_SPECIFIC_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_SPECIFIC_HPP

#include <cstdint>
#include "vendor.h"

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

namespace mk60d10
{

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

} // namespace mk60d10

namespace specific = mk60d10;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_SPECIFIC_HPP
#endif

