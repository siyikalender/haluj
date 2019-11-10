/// \file bsp.cpp
/// BSP for L03MCK100 board with 50MHz oscillator and KV31F12 device
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
/// \date   2019

#include "vendor.h"
#include "mcg.hpp"

extern "C" void set_sys_dividers(uint32_t outdiv1, uint32_t outdiv2, uint32_t outdiv3, uint32_t outdiv4);

using namespace haluj::base::devices::arm::kinetis;

constexpr uint32_t  c_crystal_freq    = 50000000U;
constexpr bool      c_high_gain       = false;
constexpr bool      c_use_crystal     = false;
constexpr uint32_t  c_divisor         = 20U;
constexpr uint32_t  c_multiplier      = 40U;
constexpr uint32_t  c_mcg_out_freq    = pll_frequency(c_crystal_freq, c_divisor, c_multiplier);
constexpr uint32_t  c_core_divisor    = 1U;
constexpr uint32_t  c_bus_divisor     = 2U;
constexpr uint32_t  c_flexbus_divisor = 2U;
constexpr uint32_t  c_flash_divisor   = 4U;

extern "C" 
void bsp_initialize_clock(const uint32_t p_default_clock)
{
  set_sys_dividers(clkdiv<c_core_divisor>(), 
                   clkdiv<c_bus_divisor>(), 
                   clkdiv<c_flexbus_divisor>(), 
                   clkdiv<c_flash_divisor>());
  
  if (init_pll<c_crystal_freq, 
                    c_high_gain, 
                    c_use_crystal, 
                    c_divisor, 
                    c_multiplier>())
  {  
    SystemCoreClock = c_mcg_out_freq / c_core_divisor;
  }
  else
  {
    SystemCoreClock = p_default_clock / c_core_divisor;
  }
}
