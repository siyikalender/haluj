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
