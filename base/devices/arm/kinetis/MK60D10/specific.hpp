/// \file specific.hpp
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

#ifndef _HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_HPP
#define _HALUJ_BASE_DEVICES_ARM_KINETIS_MK60D10_HPP

#include <cstdint>

#include "vendor.h"
#include "port.hpp"
#include "uart.hpp"

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

port<A>  port_a;
port<B>  port_b;
port<C>  port_c;
port<D>  port_d;
port<E>  port_e;

// UARTS 

struct U0
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_UART0_MASK;  
  static constexpr UART_Type*     uart_addr = UART0;
};

struct U1
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_UART1_MASK;  
  static constexpr UART_Type*     uart_addr = UART1;
};

struct U2
{
  static constexpr reg_addr_type  scgc_addr = &(SIM->SCGC4);
  static constexpr uint32_t       scgc_mask = SIM_SCGC4_UART2_MASK;  
  static constexpr UART_Type*     uart_addr = UART2;
};

uart<U0>  uart_0;
uart<U1>  uart_1;
uart<U2>  uart_2;

} // namespace mk60d10

namespace specific = mk60d10;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// _HALUJ_BASE_DEVICES_KINETIS_MK60D10_SPECIFIC_HPP
#endif

