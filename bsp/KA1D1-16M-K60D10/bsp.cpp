#include "vendor.h"
#include "mcg.hpp"

extern "C" void set_sys_dividers(uint32_t outdiv1, uint32_t outdiv2, uint32_t outdiv3, uint32_t outdiv4);

constexpr uint32_t  c_crystal_freq    = 16000000U;
constexpr bool      c_high_gain       = true;
constexpr bool      c_use_crystal     = true;
constexpr uint32_t  c_divisor         = 8U;
constexpr uint32_t  c_multiplier      = 50U;
constexpr uint32_t  c_mcg_out_freq    = pll_frequency(c_crystal_freq, c_divisor, c_multiplier);
constexpr uint32_t  c_core_divisor    = 1U;
constexpr uint32_t  c_bus_divisor     = 2U;
constexpr uint32_t  c_flexbus_divisor = 2U;
constexpr uint32_t  c_flash_divisor   = 4U;

using namespace haluj::base::devices::arm::kinetis;

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
