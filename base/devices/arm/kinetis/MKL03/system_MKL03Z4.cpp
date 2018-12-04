/// \file system_MKVL03Z4.cpp
/// System initialization functions for MKL03Z4 device family
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

#include "vendor.h"

extern "C" void disable_watchdog()
{
  SIM->COPC = (uint32_t) 0x00u;
}

template<uint32_t Value>
constexpr uint32_t clkdiv()
{
  static_assert((Value >= 1) && (Value <= 16), "Invalid clkdiv value");
  return Value - 1U;
}

extern "C" void set_sys_dividers(uint32_t outdiv1, uint32_t outdiv2, uint32_t outdiv3, uint32_t outdiv4);

constexpr uint32_t  c_device_clock    = 48000000U;
constexpr uint32_t  c_divisor         = 1U;
constexpr uint32_t  c_core_divisor    = 1U;
constexpr uint32_t  c_flash_divisor   = 4U;

uint32_t SystemCoreClock = c_device_clock;

using namespace haluj::base::devices::arm::kinetis;

extern "C" void system_init()
{
  // Set system prescalars
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(clkdiv(c_core_divisor)) |
                 SIM_CLKDIV1_OUTDIV4(clkdiv(c_flash_divisor));
  // Set FCR Divisor
  MCG->SC = MCG_SC_FCRDIV(c_divisor);   
  // Enable HIRC clock source
  MCG->MC = MCG_MC_HIRCEN_MASK;    
  // Disable External reference
  OSC->CR = (uint32_t)0x00u;
  MCG->C2 = MCG_C2_IRCS_MASK;         
  // Switch to HIRC Mode
  MCG->C1 = MCG_C1_CLKS(0x00) | MCG_C1_IRCLKEN_MASK;
  // Wait for clock source is set to the HIRC clock.
  while((MCG->S & MCG_S_CLKST_MASK) != 0x00u) {} 

  SystemCoreClock = c_device_clock / c_core_divisor;
}

