#include "vendor.h"
#include "mcg.hpp"

extern "C" void disable_watchdog()
{
  /* WDOG->UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* Key 1 */
  /* WDOG->UNLOCK: WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* Key 2 */
  /* WDOG->STCTRLH: ?=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,?=0,?=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = WDOG_STCTRLH_BYTESEL(0x00) |
                 WDOG_STCTRLH_WAITEN_MASK |
                 WDOG_STCTRLH_STOPEN_MASK |
                 WDOG_STCTRLH_ALLOWUPDATE_MASK |
                 WDOG_STCTRLH_CLKSRC_MASK |
                 0x0100U;
}

extern "C" void set_sys_dividers(uint32_t outdiv1, uint32_t outdiv2, uint32_t outdiv3, uint32_t outdiv4);

uint32_t SystemCoreClock = 20971520U;

constexpr uint32_t 
pll_frequency(const uint32_t p_frequency,
              const uint32_t p_divisor,
              const uint32_t p_multiplier)
{
  return (p_frequency / p_divisor) * p_multiplier;
}

template<uint32_t Value>
constexpr uint32_t clkdiv()
{
  static_assert((Value >= 1) && (Value <= 16), "Invalid clkdiv value");
  return Value - 1U;
}

constexpr uint32_t  c_crystal_freq    = 16000000U;
constexpr bool      c_high_gain       = true;
constexpr bool      c_external_ref    = true;
constexpr uint32_t  c_divisor         = 8U;
constexpr uint32_t  c_multiplier      = 50U;
constexpr uint32_t  c_mcg_out_freq    = pll_frequency(c_crystal_freq, c_divisor, c_multiplier);
constexpr uint32_t  c_core_divisor    = 1U;
constexpr uint32_t  c_bus_divisor     = 2U;
constexpr uint32_t  c_flexbus_divisor = 2U;
constexpr uint32_t  c_flash_divisor   = 4U;

using namespace haluj::base::devices::arm::kinetis;

extern "C" void system_init()
{
  // disable MPU
  MPU_CESR &= ~MPU_CESR_VLD_MASK;
  // initialize PLL 
  set_sys_dividers(clkdiv<c_core_divisor>(), 
                   clkdiv<c_bus_divisor>(), 
                   clkdiv<c_flexbus_divisor>(), 
                   clkdiv<c_flash_divisor>());
  
  if (init_pll<c_crystal_freq, 
                    c_high_gain, 
                    c_external_ref, 
                    c_divisor, 
                    c_multiplier>())
  {  
    SystemCoreClock = c_mcg_out_freq / c_core_divisor;
  }
}

