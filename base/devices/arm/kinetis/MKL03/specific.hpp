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
/// \brief Sets baud rate registers. 
inline uint32_t 
set_baud_rate(LPUART_Type&    p_device, 
              const uint32_t  p_peripheral_clock, 
              const uint32_t  p_baud_rate) 
{

  uint32_t div_   = p_peripheral_clock / p_baud_rate;

  bool quit = false;

  uint32_t  u     = 32U,
            min_r = 32U, 
            min_u = 32U;

  for (; u>=4U && !quit; u--)
  {
    auto r = div_ % u;
    
    if (r < min_r)
    {
      min_r = r;
      min_u = u;
    }
    
    quit = (r == 0);
  }
  uint32_t sbr    = (p_peripheral_clock / (p_baud_rate * min_u));
  uint32_t osr    = min_u - 1;

  uint32_t value  = p_device.BAUD;
  value &= ~LPUART_BAUD_SBR_MASK & ~LPUART_BAUD_OSR_MASK;
  value |= LPUART_BAUD_OSR(osr) | LPUART_BAUD_SBR(sbr);
  p_device.BAUD = value;

  return sbr;
}

/// \fn set_fractional_divider
/// \brief This function is not supported by the device
inline uint32_t 
set_fractional_divider(UART_Type&      p_device, 
                       const uint32_t  p_peripheral_clock, 
                       const uint32_t  p_baud_rate,
                       const uint32_t  p_sbr) 
{
  return 0;
}

/// \fn set_bits
/// \brief Sets data bits. 
inline void
set_bits(UART_Type&       p_device, 
         const uint32_t   p_value)
{
  uint32_t value = p_device.CTRL;
  
  switch(p_value)
  {
  default:
    value = mask_clear(value, LPUART_CTRL_M_MASK);
    break;
  case 9:
    value = mask_set(value, LPUART_CTRL_M_MASK);
    break;
  case 10:
    // Not supported yet
    break;
  }
  
  p_device.CTRL = value;  
}

/// \fn set_parity
/// \brief Sets parity. 
inline void
set_parity(UART_Type&       p_device, 
           const uint32_t   p_value)
{
  uint32_t value = p_device.CTRL;
  
  value = mask_clear(value, LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK);

  switch(p_value)
  {
  default: // 0 none
    break;
  case 1:  // 1 even
    value = mask_set(value, LPUART_CTRL_PE_MASK);
    break;
  case 2:  // 2 odd
    value = mask_set(value, LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK);
    break;
  }

  p_device.CTRL = value;
}

/// \fn set_stop_bits
/// \brief Sets stop bits.
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

inline bool 
is_error(UART_Type& p_device) 
{
  return (p_device.STAT & (LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK | LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK)) != 0;
}

inline void
clear_errors(UART_Type& p_device) 
{
  p_device.STAT = mask_set(p_device.STAT,  LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK | LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);
}

inline void start(UART_Type&       p_device)
{
  p_device.CTRL = 
    mask_set(p_device.CTRL, 
             LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);
}

inline void stop(UART_Type&       p_device)
{
  p_device.CTRL = mask_clear(p_device.CTRL, 
                             LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);
  while((p_device.CTRL & (LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK)) != 0);
}

inline void clear(UART_Type&       p_device)
{
  // p_device.STAT = LPUART_STAT_LBKDIF_MASK | LPUART_STAT_RXEDGIF_MASK  | 
  //                 LPUART_STAT_IDLE_MASK   | LPUART_STAT_OR_MASK       | 
  //                 LPUART_STAT_NF_MASK     | LPUART_STAT_FE_MASK       | 
  //                 LPUART_STAT_PF_MASK     | LPUART_STAT_MA1F_MASK     |
  //                 LPUART_STAT_MA2F_MASK;
  p_device.CTRL = 0;
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

/// \fn start
/// \brief starts ADC
inline void start(ADC_Type&       p_device,
                  const unsigned  p_index,
                  const unsigned  p_channel, 
                  const bool      /* p_is_differential */)
{
  p_device.SC1[p_index] = ADC_SC1_ADCH(p_channel);
}

/// \fn configure
/// \brief Configures ADC
inline void
configure(ADC_Type& p_device)
{
  p_device.CFG1 =  ADC_CFG1_ADIV(3)     | // 2^3=8 /8
                   ADC_CFG1_ADLSMP_MASK |
                   ADC_CFG1_MODE(0)     |  // 8-Bit Mode
                   ADC_CFG1_ADICLK(0);   // Bus Clock

  p_device.CFG2 = // ADC_CFG2_MUXSEL_MASK | // Enable for SE_b inputs
                 ADC_CFG2_ADLSTS(0x00); // ADLSTS_20

//  p_device.CV1  = 0x12u;
//  p_device.CV2  = 0x78u;
//  p_device.SC2  = ADC_SC2_REFSEL(0x00); // REFSEL_EXT (VREFH ve VREFL)
  p_device.SC2  = 0x00; // REFSEL_EXT (VREFH ve VREFL)

//  p_device.SC3  = ADC_SC3_AVGE_MASK       |
//                  ADC_SC3_AVGS(0x00); // 32 Sample Average
  p_device.SC3  = 0x00;
}

/// \fn calibrate
/// \brief Calibrates ADC
inline void 
calibrate(ADC_Type& p_device)
{
  unsigned short cal_var;

//  p_device.SC2 &= ~ADC_SC2_ADTRG_MASK; // Enable Software Conversion Trigger for Calibration Process    - adc_addr()->SC2 = adc_addr()->SC2 | ADC_SC2_ADTRGW(0);
  //p_device.SC3 &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); // set single conversion, clear avgs bitfield for next writing
  p_device.SC3 |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(0x03) );  // Turn averaging ON and set at max value ( 32 )

  p_device.SC3 |= ADC_SC3_CAL_MASK ;      // Start CAL
  while ( (p_device.SC1[0] & ADC_SC1_COCO_MASK ) == 0 ); // Wait calibration end

  if ((p_device.SC3 & ADC_SC3_CALF_MASK) == ADC_SC3_CALF_MASK )
  {
    return;    // Check for Calibration fail error and return
  }
  // Calculate plus-side calibration
  cal_var = 0x00;

  cal_var =  p_device.CLP0;
  cal_var += p_device.CLP1;
  cal_var += p_device.CLP2;
  cal_var += p_device.CLP3;
  cal_var += p_device.CLP4;
  cal_var += p_device.CLPS;

  cal_var = cal_var/2;
  cal_var |= 0x8000; // Set MSB

  p_device.PG = ADC_PG_PG(cal_var);

  p_device.SC3 &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */
}  

/// \fn is_data_available
/// \brief Checks Conversdion is completed
inline bool is_data_available(ADC_Type& p_device, const unsigned p_index)
{
  return ((p_device.SC1[p_index] & ADC_SC1_COCO_MASK ) == ADC_SC1_COCO_MASK);
}

/// \fn read
/// \brief Reads the conversion result
inline uint32_t read(ADC_Type& p_device, const unsigned p_index)
{
  return p_device.R[p_index];
}

/////////////////////////////////////////////////////////////

inline void start_watchdog()
{
  SIM->COPC = SIM_COPC_COPT(1); // LPO:1Khz , 2^5 clock -> 32mS
}

inline void stop_watchdog()
{
  SIM->COPC = 0x00u;
}

inline void reset_watchdog()
{
  SIM->SRVCOP = 0x55u;
  SIM->SRVCOP = 0xAAu;
}

/////////////////////////////////////////////////////////////

} // namespace mkl03

namespace specific = mkl03;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_MKL03_SPECIFIC_HPP
#endif

