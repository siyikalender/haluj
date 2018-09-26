#include "clock.hpp"
#include "vendor.h"

#define SYSTICK_HZ      100000U
#define SYSTICK_US      (1000000U/SYSTICK_HZ)

// volatile std::atomic<int64_t>   now__;

volatile int64_t  now__   = 0ULL;

volatile bool g_flag = false;

extern "C"
#ifndef DONT_USE_RAM_CODE
__attribute((section(".ramcode")))
#endif
void SysTick_isr(void)
{
  g_flag = true;
  now__ += SYSTICK_US; // 64 bit update may not be safe
  asm ("" : : : "memory"); // gcc specific
}

namespace haluj
{

namespace base
{

namespace devices
{

namespace arm
{

precision_clock::time_point 
precision_clock::initialize()
{
  SysTick_Config(SystemCoreClock / SYSTICK_HZ);
  return 
    precision_clock::time_point(precision_clock::duration(0ULL));  
}

precision_clock::time_point 
precision_clock::now()
{
  precision_clock::time_point    result;
  __disable_irq();
  result = precision_clock::time_point(precision_clock::duration(static_cast<int64_t>(now__)));
  __enable_irq();
  return result;
}

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj
