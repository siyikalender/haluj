/// \file peripherals.hpp
/// specific peripheral definitions for Kinetis MK60D10 family devices
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_PERIPHERALS_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_PERIPHERALS_HPP

#include <cstdint>

#include "vendor.h"
#include "port.hpp"
#include "uart.hpp"
#include "spi.hpp"
#include "i2c.hpp"
#include "adc.hpp"
#include "vref.hpp"

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

// PORTS 

struct A
{
  static constexpr intptr_t   sim_base    = SIM_BASE;
  static constexpr intptr_t   port_base   = PORTA_BASE;
  static constexpr intptr_t   gpio_base   = PTA_BASE;
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTA_MASK;
  
  static reg_addr_type  scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC5); 
  }
};

struct B
{
  static constexpr intptr_t   sim_base    = SIM_BASE;
  static constexpr intptr_t   port_base   = PORTB_BASE;
  static constexpr intptr_t   gpio_base   = PTB_BASE;
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTB_MASK;

  static reg_addr_type  scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC5); 
  }
};

struct C
{
  static constexpr intptr_t   sim_base    = SIM_BASE;
  static constexpr intptr_t   port_base   = PORTC_BASE;
  static constexpr intptr_t   gpio_base   = PTC_BASE;
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTC_MASK;

  static reg_addr_type  scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC5); 
  }
};

struct D
{
  static constexpr intptr_t   sim_base    = SIM_BASE;
  static constexpr intptr_t   port_base   = PORTD_BASE;
  static constexpr intptr_t   gpio_base   = PTD_BASE;
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTD_MASK;

  static reg_addr_type  scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC5); 
  }
};

struct E
{
  static constexpr intptr_t   sim_base    = SIM_BASE;
  static constexpr intptr_t   port_base   = PORTE_BASE;
  static constexpr intptr_t   gpio_base   = PTE_BASE;
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTE_MASK;

  static reg_addr_type  scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC5); 
  }
};

typedef port<A>  port_a;
typedef port<B>  port_b;
typedef port<C>  port_c;
typedef port<D>  port_d;
typedef port<E>  port_e;

// UARTS 

struct U0
{
  static constexpr intptr_t   sim_base        = SIM_BASE;
  static constexpr intptr_t   uart_base       = UART0_BASE;
  static constexpr uint32_t   scgc_mask       = SIM_SCGC4_UART0_MASK;  
  static constexpr bool       uses_core_clock = true;

  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC4); 
  }
};

struct U1
{
  static constexpr intptr_t   sim_base        = SIM_BASE;
  static constexpr intptr_t   uart_base       = UART1_BASE;
  static constexpr uint32_t   scgc_mask       = SIM_SCGC4_UART1_MASK;  
  static constexpr bool       uses_core_clock = true;

  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC4); 
  }
};

struct U2
{
  static constexpr intptr_t   sim_base        = SIM_BASE;
  static constexpr intptr_t   uart_base       = UART2_BASE;
  static constexpr uint32_t   scgc_mask       = SIM_SCGC4_UART2_MASK;  
  static constexpr bool       uses_core_clock = true;

  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC4); 
  }
};

typedef uart<U0>  uart_0;
typedef uart<U1>  uart_1;
typedef uart<U2>  uart_2;

// SPI 
struct _SPI0
{
  static constexpr intptr_t       sim_base        = SIM_BASE;
  static constexpr intptr_t       spi_base        = SPI0_BASE;
  static constexpr uint32_t       scgc_mask       = SIM_SCGC6_SPI0_MASK;  

  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC6); 
  }
};

struct _SPI1
{
  static constexpr intptr_t       sim_base        = SIM_BASE;
  static constexpr intptr_t       spi_base        = SPI1_BASE;
  static constexpr uint32_t       scgc_mask       = SIM_SCGC6_SPI1_MASK;  

  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC6); 
  }
};

typedef spi<_SPI0>  spi_0;
typedef spi<_SPI1>  spi_1;

// SPI 
struct _I2C0
{
  static constexpr intptr_t sim_base  = SIM_BASE;
  static constexpr intptr_t i2c_base  = I2C0_BASE;
  static constexpr uint32_t scgc_mask = SIM_SCGC4_I2C0_MASK;    
  
  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC4); 
  }  
};

struct _I2C1
{
  static constexpr intptr_t sim_base  = SIM_BASE;
  static constexpr intptr_t i2c_base  = I2C1_BASE;
  static constexpr uint32_t scgc_mask = SIM_SCGC4_I2C1_MASK;    
  
  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC4); 
  }  
};

typedef i2c<_I2C0>  i2c_0;
typedef i2c<_I2C1>  i2c_1;

// ADC
struct _ADC0
{
  static constexpr intptr_t sim_base  = SIM_BASE;
  static constexpr intptr_t adc_base  = ADC0_BASE;
  static constexpr uint32_t scgc_mask = SIM_SCGC6_ADC0_MASK;    
  
  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC6); 
  }    
};

struct _ADC1
{
  static constexpr intptr_t sim_base  = SIM_BASE;
  static constexpr intptr_t adc_base  = ADC1_BASE;
  static constexpr uint32_t scgc_mask = SIM_SCGC6_ADC1_MASK;    
  
  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC6); 
  }    
};

typedef adc<_ADC0>  adc_0;
typedef adc<_ADC1>  adc_1;

// VREF 
struct _VREF_0
{
  static constexpr intptr_t sim_base  = SIM_BASE;
  static constexpr intptr_t vref_base = VREF_BASE;
  static constexpr uint32_t scgc_mask = SIM_SCGC4_VREF_MASK;    
  
  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC4); 
  }    
};

typedef vref<_VREF_0>  vref_0;

} // namespace mkv31f

namespace specific = mkv31f;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_PERIPHERALS_HPP
#endif

