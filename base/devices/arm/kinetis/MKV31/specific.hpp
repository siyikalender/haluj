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

namespace mkv31f
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

inline uint32_t 
read(UART_Type& p_device) 
{
  return p_device.D;
}

inline bool 
is_rx_available(UART_Type& p_device) 
{
  return mask_test(p_device.S1, UART_S1_RDRF_MASK);
}

inline void 
write(UART_Type&       p_device, const uint8_t p_data) 
{
  p_device.D = p_data;
}

inline bool
is_tx_ready(UART_Type& p_device) 
{
  return mask_test(p_device.S1, UART_S1_TDRE_MASK);
}  

inline bool 
is_error(UART_Type& p_device) 
{
  return (p_device.S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) != 0;
}

inline void
clear_errors(UART_Type& p_device) 
{
  volatile uint32_t dummy = p_device.D;
}

inline void 
start(UART_Type& p_device)
{
  p_device.C2 = 
    mask_set(p_device.C2, 
             UART_C2_TE_MASK | UART_C2_RE_MASK);
}

inline void 
stop(UART_Type& p_device)
{
  p_device.C2 = 
    mask_clear(p_device.C2, 
               UART_C2_TE_MASK | UART_C2_RE_MASK);
}

inline void 
clear(UART_Type& p_device)
{
  p_device.C1  = 0;
  p_device.C4  = 0;
  p_device.BDH = 0;
}

/////////////////////////////////////////////////////////////

/// \fn clear
/// \brief Clears modified SPI registers prior configuration
inline void
clear(SPI_Type& p_device)
{
  p_device.MCR     = SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK;
  p_device.CTAR[0] = SPI_CTAR_PASC(2)    | 
                     SPI_CTAR_ASC(1)     | 
                     SPI_CTAR_PCSSCK(2)  | 
                     SPI_CTAR_CSSCK(1)   | 
                     SPI_CTAR_PBR(2)     | 
                     SPI_CTAR_BR(4);
}

/// \fn start
/// \brief Starts SPI transfer
inline void
start(SPI_Type& p_device)
{
  p_device.MCR |= SPI_MCR_HALT_MASK;
}

/// \fn stop
/// \brief Starts SPI transfer
inline void
stop(SPI_Type& p_device)
{
  p_device.MCR &= ~SPI_MCR_HALT_MASK;
}

/////////////////////////////////////////////////////////////

inline void clear_iicif(I2C_Type& p_device)
{
  p_device.S |= I2C_S_IICIF_MASK;
}

inline void clear_arbl(I2C_Type& p_device)
{
  p_device.S |= I2C_S_ARBL_MASK;
}

inline uint32_t status(I2C_Type& p_device)
{
  return p_device.S;
}

/////////////////////////////////////////////////////////////

/// \fn start
/// \brief starts ADC
inline void start(ADC_Type&       p_device, 
                  const unsigned  p_channel, 
                  const bool      p_is_differential)
{
  if (p_is_differential)
  {
    p_device.SC1[0] = ADC_SC1_DIFF_MASK | ADC_SC1_ADCH(p_channel);
  }
  else
  {
    p_device.SC1[0] = ADC_SC1_ADCH(p_channel);
  }
}

/// \fn configure
/// \brief Configures ADC
inline void
configure(ADC_Type& p_device)
{
  p_device.CFG1 =  ADC_CFG1_ADIV(3)     | // 2^3=8 /8
                   ADC_CFG1_ADLSMP_MASK |
                   ADC_CFG1_MODE(0x03)  |  // 16-Bit Mode
                   ADC_CFG1_ADICLK(0);   // Bus Clock

  p_device.CFG2 = // ADC_CFG2_MUXSEL_MASK | // Enable for SE_b inputs
                 ADC_CFG2_ADLSTS(0x00); // ADLSTS_20

  p_device.CV1  = 0x1234u;
  p_device.CV2  = 0x5678u;
  p_device.SC2  = ADC_SC2_REFSEL(0x00); // REFSEL_EXT (VREFH ve VREFL)

  p_device.SC3  = ADC_SC3_AVGE_MASK       |
                  ADC_SC3_AVGS(0x00); // 32 Sample Average

  #if defined(ADC_PGA_PGAG)
  p_device.PGA  = ADC_PGA_PGAG(0); // Gain 2^0
  #endif
}

/// \fn calibrate
/// \brief Calibrates ADC
inline void 
calibrate(ADC_Type& p_device)
{
  unsigned short cal_var;

  p_device.SC2 &= ~ADC_SC2_ADTRG_MASK; // Enable Software Conversion Trigger for Calibration Process    - adc_addr()->SC2 = adc_addr()->SC2 | ADC_SC2_ADTRGW(0);
  p_device.SC3 &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); // set single conversion, clear avgs bitfield for next writing
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

  // Calculate minus-side calibration
  cal_var = 0x00;

  cal_var =  p_device.CLM0;
  cal_var += p_device.CLM1;
  cal_var += p_device.CLM2;
  cal_var += p_device.CLM3;
  cal_var += p_device.CLM4;
  cal_var += p_device.CLMS;

  cal_var = cal_var/2;

  cal_var |= 0x8000; // Set MSB

  #ifdef ADC_MG_MG
  p_device.MG = ADC_MG_MG(cal_var);
  #else 
  #warning "Skipping adc_addr()->MG = ADC_MG_MG(cal_var)"
  #endif

  p_device.SC3 &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */
}  

/// \fn is_data_available
/// \brief Checks Conversdion is completed
inline bool is_data_available(ADC_Type& p_device, const unsigned p_channel)
{
  return ((p_device.SC1[p_channel] & ADC_SC1_COCO_MASK ) == ADC_SC1_COCO_MASK);
}

/// \fn read
/// \brief Reads the conversion result
inline uint32_t read(ADC_Type& p_device, const unsigned p_channel)
{
  return p_device.R[p_channel];
}

} // namespace mkv31f

namespace specific = mkv31f;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_SPECIFIC_HPP
#endif

