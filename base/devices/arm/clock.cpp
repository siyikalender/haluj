/// \file clock.hpp
/// implementation of clock 
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
