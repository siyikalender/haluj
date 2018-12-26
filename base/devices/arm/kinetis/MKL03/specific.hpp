/// \file specific.hpp
/// specific definitions for Kinetis MKL03 family devices
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_MKL03_SPECIFIC_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_MKL03_SPECIFIC_HPP

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

namespace mkl03
{

/// \fn set_baud_rate
/// \brief Sets baud rate registers. Assumed UART BDH register is cleared
inline uint32_t 
set_baud_rate(LPUART_Type&    p_device, 
              const uint32_t  p_peripheral_clock, 
              const uint32_t  p_baud_rate) 
{
  uint32_t value  = p_device.BAUD;
  uint32_t osr    = ((value & LPUART_BAUD_OSR_MASK) >> LPUART_BAUD_OSR_SHIFT) + 1;
  uint32_t sbr    = (p_peripheral_clock / (p_baud_rate * osr));
  value &= ~LPUART_BAUD_SBR_MASK;
  value |= sbr;
  p_device.BAUD = value;
  return sbr;
}

/// \fn set_fractional_divider
/// \brief Sets fractional divider. Assumed UART C4 register is cleared
inline uint32_t 
set_fractional_divider(UART_Type&      p_device, 
                       const uint32_t  p_peripheral_clock, 
                       const uint32_t  p_baud_rate,
                       const uint32_t  p_sbr) 
{
  return 0;
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
    break;
  case 10:
    break;
  }
}

/// \fn set_parity
/// \brief Sets parity. Assumed UART C1 is cleared
inline void
set_parity(UART_Type&       p_device, 
           const uint32_t   p_value)
{
  uint32_t value = p_device.CTRL;
  
  value &= ~(LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK);

  switch(p_value)
  {
  default: // 0 none
    break;
  case 1:
    value |= LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK;
    break;
  case 2:
    value |= LPUART_CTRL_PE_MASK ;
    break;
  }

  p_device.CTRL = value;
}

/// \fn set_stop_bits
/// \brief Sets stop bits. Assumed UART BDH SBNS field is cleared
inline void
set_stop_bits(UART_Type&       p_device, 
              const uint32_t   p_value)
{
  uint32_t value  = p_device.BAUD;
  
  value &= ~LPUART_BAUD_SBNS_MASK;  
  
  switch(p_value)
  {
  default:  // one
    break;
  case 1:   // two
    value |= LPUART_BAUD_SBNS_MASK;
    break;
  }
  
  p_device.BAUD = value;
}

inline uint32_t read(UART_Type&       p_device) 
{
  return p_device.DATA;
}

inline bool is_rx_available(UART_Type&       p_device) 
{
  return mask_test(p_device.STAT, LPUART_STAT_RDRF_MASK);
}

inline  void write(UART_Type& p_device, const uint8_t p_data) 
{
  p_device.DATA = p_data;
}

inline bool is_tx_ready(UART_Type&       p_device) 
{
  return mask_test(p_device.STAT, LPUART_STAT_TDRE_MASK);
}  

inline void start(UART_Type&       p_device)
{
  p_device.CTRL = 
    mask_set(p_device.CTRL, 
             LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);
}

inline void stop(UART_Type&       p_device)
{
  p_device.CTRL = 
    mask_clear(p_device.CTRL, 
               LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);
}

inline void clear(UART_Type&       p_device)
{
  // p_device.STAT = LPUART_STAT_LBKDIF_MASK | LPUART_STAT_RXEDGIF_MASK  | 
  //                 LPUART_STAT_IDLE_MASK   | LPUART_STAT_OR_MASK       | 
  //                 LPUART_STAT_NF_MASK     | LPUART_STAT_FE_MASK       | 
  //                 LPUART_STAT_PF_MASK     | LPUART_STAT_MA1F_MASK     |
  //                 LPUART_STAT_MA2F_MASK;
  // p_device.CTRL = 0;
}

/////////////////////////////////////////////////////////////

/// \fn clear
/// \brief Clears modified SPI registers prior configuration
inline void
clear(SPI_Type& p_device)
{
// p_device.MCR     = SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK;
// p_device.CTAR[0] = SPI_CTAR_PASC(2)    | 
//                    SPI_CTAR_ASC(1)     | 
//                    SPI_CTAR_PCSSCK(2)  | 
//                    SPI_CTAR_CSSCK(1)   | 
//                    SPI_CTAR_PBR(2)     | 
//                    SPI_CTAR_BR(4);
}

/// \fn start
/// \brief Starts SPI transfer
inline void
start(SPI_Type& p_device)
{
//  p_device.MCR |= SPI_MCR_HALT_MASK;
}

/// \fn stop
/// \brief Starts SPI transfer
inline void
stop(SPI_Type& p_device)
{
//  p_device.MCR &= ~SPI_MCR_HALT_MASK;
}

/////////////////////////////////////////////////////////////

inline void clear_iicif(I2C_Type& p_device)
{
  p_device.S1 |= I2C_S_IICIF_MASK;
}

inline void clear_arbl(I2C_Type& p_device)
{
  p_device.S1 |= I2C_S_ARBL_MASK;
}

inline uint32_t status(I2C_Type& p_device)
{
  return p_device.S1;
}

/////////////////////////////////////////////////////////////

inline void start(ADC_Type&       p_device, 
                  const unsigned  p_channel, 
                  const bool      p_is_differential)
{}

/// \fn configure
/// \brief Configures ADC
inline void
configure(ADC_Type& p_device)
{}

/// \fn calibrate
/// \brief Calibrates ADC
inline void 
calibrate(ADC_Type& p_device)
{} 

/// \fn is_data_available
/// \brief Checks Conversdion is completed
inline bool is_data_available(ADC_Type& p_device, const unsigned p_channel)
{
  return true;
}

/// \fn read
/// \brief Reads the conversion result
inline uint32_t read(ADC_Type& p_device, const unsigned p_channel)
{
  return 0;
}

} // namespace mkl03

namespace specific = mkl03;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_MKL03_SPECIFIC_HPP
#endif

