/// \file core.hpp
/// Core related functions
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_CORE_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_CORE_HPP

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

inline uint32_t mcg_out_clock()
{
  auto temp = SIM->CLKDIV1;
  temp &=  SIM_CLKDIV1_OUTDIV1_MASK;
  temp >>= SIM_CLKDIV1_OUTDIV1_SHIFT;
  temp++;
  return SystemCoreClock * temp;
}

inline uint32_t system_core_clock()
{
  return SystemCoreClock;
}

inline uint32_t system_bus_clock()
{
  auto temp = SIM->CLKDIV1;
  temp &=  SIM_CLKDIV1_OUTDIV2_MASK;
  temp >>= SIM_CLKDIV1_OUTDIV2_SHIFT;
  temp++;
  return mcg_out_clock() / temp;
}

inline uint32_t system_flexbus_clock()
{
  auto temp = SIM->CLKDIV1;
  temp &=  SIM_CLKDIV1_OUTDIV3_MASK;
  temp >>= SIM_CLKDIV1_OUTDIV3_SHIFT;
  temp++;
  return mcg_out_clock() / temp;
}

inline uint32_t system_flash_clock()
{
  auto temp = SIM->CLKDIV1;
  temp &=  SIM_CLKDIV1_OUTDIV4_MASK;
  temp >>= SIM_CLKDIV1_OUTDIV4_SHIFT;
  temp++;
  return mcg_out_clock() / temp;
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj


#endif // HALUJ_BASE_DEVICE_KINETIS_
