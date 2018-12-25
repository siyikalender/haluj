/// \file peripherals.hpp
/// specific peripheral definitions for Kinetis MKL03 family devices
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_MKL03_PERIPHERALS_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_MKL03_PERIPHERALS_HPP

#include <cstdint>

#include "vendor.h"
#include "port.hpp"
#include "uart.hpp"
// #include "spi.hpp"
#include "i2c.hpp"
#include "tpm.hpp"

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

typedef port<A>  port_a;
typedef port<B>  port_b;

// UARTS 

struct U0
{
  static constexpr intptr_t   sim_base        = SIM_BASE;
  static constexpr intptr_t   uart_base       = LPUART0_BASE;
  static constexpr uint32_t   scgc_mask       = SIM_SCGC5_LPUART0_MASK;  
  static constexpr bool       uses_core_clock = true;

  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC5); 
  }  
};

typedef uart<U0>    lpuart_0;

// SPI 
// I2C

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

typedef i2c<_I2C0>  i2c_0;

// TPM

struct _TPM0
{
  static constexpr intptr_t sim_base  = SIM_BASE;
  static constexpr intptr_t tpm_base  = TPM0_BASE;
  static constexpr uint32_t scgc_mask = SIM_SCGC6_TPM0_MASK;      
  
  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC6); 
  }
};

struct _TPM1
{
  static constexpr intptr_t sim_base  = SIM_BASE;
  static constexpr intptr_t tpm_base  = TPM1_BASE;
  static constexpr uint32_t scgc_mask = SIM_SCGC6_TPM1_MASK;      
  
  static reg_addr_type scgc_addr() 
  { 
    return &(reinterpret_cast<SIM_Type*>(sim_base)->SCGC6); 
  }
};

typedef tpm<_TPM0>  tpm_0; 
typedef tpm<_TPM1>  tpm_1;

} // namespace mkl03

namespace specific = mkl03;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_MKL03_PERIPHERALS_HPP
#endif

