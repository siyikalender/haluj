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

// PORTS 

struct A
{
  static constexpr reg_addr_type  scgc_addr   = &(SIM->SCGC5);
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTA_MASK;
  static constexpr PORT_Type* port_addr   = PORTA;
  static constexpr GPIO_Type* gpio_addr   = GPIOA;
};

struct B
{
  static constexpr reg_addr_type  scgc_addr   = &(SIM->SCGC5);
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTB_MASK;
  static constexpr PORT_Type* port_addr   = PORTB;
  static constexpr GPIO_Type* gpio_addr   = GPIOB;
};

struct C
{
  static constexpr reg_addr_type  scgc_addr   = &(SIM->SCGC5);
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTC_MASK;
  static constexpr PORT_Type* port_addr   = PORTC;
  static constexpr GPIO_Type* gpio_addr   = GPIOC;
};

struct D
{
  static constexpr reg_addr_type  scgc_addr   = &(SIM->SCGC5);
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTD_MASK;
  static constexpr PORT_Type* port_addr   = PORTD;
  static constexpr GPIO_Type* gpio_addr   = GPIOD;
};

struct E
{
  static constexpr reg_addr_type  scgc_addr   = &(SIM->SCGC5);
  static constexpr uint32_t   scgc_mask   = SIM_SCGC5_PORTE_MASK;
  static constexpr PORT_Type* port_addr   = PORTE;
  static constexpr GPIO_Type* gpio_addr   = GPIOE;
};

typedef port<A>  port_a;
typedef port<B>  port_b;
typedef port<C>  port_c;
typedef port<D>  port_d;
typedef port<E>  port_e;

// UARTS 

struct U0
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_UART0_MASK;  
  static constexpr UART_Type*     uart_addr = UART0;
  static constexpr bool           uses_core_clock = true;
};

struct U1
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_UART1_MASK;  
  static constexpr UART_Type*     uart_addr = UART1;
  static constexpr bool           uses_core_clock = true;
};

struct U2
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_UART2_MASK;  
  static constexpr UART_Type*     uart_addr = UART2;
  static constexpr bool           uses_core_clock = false;
};

struct U3
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_UART3_MASK;  
  static constexpr UART_Type*     uart_addr = UART3;
  static constexpr bool           uses_core_clock = false;
};

struct U4
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC1);
  static constexpr uint32_t       scgc_mask = SIM_SCGC1_UART4_MASK;  
  static constexpr UART_Type*     uart_addr = UART4;
  static constexpr bool           uses_core_clock = false;
};

typedef uart<U0>  uart_0;
typedef uart<U1>  uart_1;
typedef uart<U2>  uart_2;
typedef uart<U4>  uart_4;

// SPI 
struct _SPI0
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC6);
  static constexpr uint32_t       scgc_mask = SIM_SCGC6_SPI0_MASK;  
  static constexpr SPI_Type*      spi_addr  = SPI0;
};

struct _SPI1
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC6);
  static constexpr uint32_t       scgc_mask = SIM_SCGC6_SPI1_MASK;  
  static constexpr SPI_Type*      spi_addr  = SPI1;
};

struct _SPI2
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC3);
  static constexpr uint32_t       scgc_mask = SIM_SCGC3_SPI2_MASK;  
  static constexpr SPI_Type*      spi_addr  = SPI2;
};

typedef spi<_SPI0>  spi_0;
typedef spi<_SPI1>  spi_1;
typedef spi<_SPI2>  spi_2;

// SPI 
struct _I2C0
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_I2C0_MASK;  
  static constexpr I2C_Type*      i2c_addr  = I2C0;
};

struct _I2C1
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_I2C1_MASK;  
  static constexpr I2C_Type*      i2c_addr  = I2C1;
};

typedef i2c<_I2C0>  i2c_0;
typedef i2c<_I2C1>  i2c_1;

} // namespace mk60d10

namespace specific = mk60d10;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_PERIPHERALS_HPP
#endif

