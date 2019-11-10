/// \file mcg.hpp
/// Multipurpose Clock Generator peripheral implementation
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

// Rewritten in C++. The source is based on the old SDK code 
// from Freescale days

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_MCG_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_MCG_HPP

#include <tuple>

#include "vendor.h"

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

typedef std::tuple<bool, unsigned>  result_t;

inline bool is_in_fei_mode()
{
  return ((((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x0) &&
            (MCG->S & MCG_S_IREFST_MASK) &&
            (!(MCG->S & MCG_S_PLLST_MASK)));
}

inline bool is_in_fbe_mode()
{
  return ( (((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) &&
           (!(MCG->S & MCG_S_IREFST_MASK)) &&
           (!(MCG->S & MCG_S_PLLST_MASK)) &&
           (!(MCG->C2 & MCG_C2_LP_MASK)));
}

inline bool is_in_pbe_mode()
{
  return ((((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) &&
           (!(MCG->S & MCG_S_IREFST_MASK)) &&
           (MCG->S & MCG_S_PLLST_MASK) &&
           (!(MCG->C2 & MCG_C2_LP_MASK)));
}

template<unsigned CrystalOscFreqHz, bool IsACrystal>
inline constexpr void check_crystal_freq()
{
  static_assert(!(CrystalOscFreqHz > 60000000),
                "STATIC_ASSERT ! Crystal/Osc frequency input can not "
                "exceed the maximum value");

  static_assert(
    !((IsACrystal) &&
     ( (CrystalOscFreqHz < 30000) ||
       ((CrystalOscFreqHz > 40000) && (CrystalOscFreqHz < 3000000)) ||
       (CrystalOscFreqHz > 32000000))),
    "STATIC_ASSERT ! Crystal frequency value is not valid range");
}

template<unsigned CrystalOscFreqHz,
         unsigned Prescaler,
         unsigned Multiplier>
inline constexpr void check_pll_parameters()
{
  // Check PLL divider settings are within spec.
  static_assert ( (Prescaler >= 1) && (Prescaler <= 25),
                  "STATIC_ASSERT ! Prescaler is not in range" );
  static_assert ( (Multiplier >= 24) && (Multiplier <= 55),
                  "STATIC_ASSERT ! Multiplier is not in range" );
  // Check PLL reference clock frequency is within spec.
  static_assert ( ((CrystalOscFreqHz / Prescaler) >= 2000000) &&
                  ((CrystalOscFreqHz / Prescaler) <= 4000000) ,
                  "STATIC_ASSERT ! Prescaled frequency is not in valid range" );
  // Check PLL output frequency is within spec.
  static_assert ( (((CrystalOscFreqHz / Prescaler) * Multiplier) >= PLL_MINIMUM_FREQ_HZ) &&
                  (((CrystalOscFreqHz / Prescaler) * Multiplier) <= PLL_MAXIMUM_FREQ_HZ),
                 "STATIC_ASSERT ! PLL frequency is not in valid range" );
}

template <unsigned Count,
          unsigned Mask,
          unsigned Shift,
          unsigned Value>
inline bool mcg_wait_status()
{
  bool result = false;
  for (unsigned i = 0; i < Count && !result; i++)
  {
    result = ((MCG->S & Mask) >> Shift ) == Value;
  }
  return result;
}

// fei_fbe: fei to fbe transition
// Sets the External Resonator or External Clock as clock source
// device operates at crystal_val after successful transition

inline result_t fei_fbe(unsigned        crystal_val,
                        bool            use_crystal,
                        bool            high_gain)
{
  unsigned char frdiv_val;
  unsigned char temp_reg;
  
  unsigned char hgo_val   = (high_gain) ? 1 : 0;
  unsigned char erefs_val = (use_crystal) ? 1 : 0;
  
  temp_reg = MCG->C2;

  temp_reg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK);
  if (crystal_val <= 40000) {
    temp_reg |=
      (MCG_C2_RANGE0(0) | (hgo_val << MCG_C2_HGO0_SHIFT) |
       (erefs_val << MCG_C2_EREFS0_SHIFT));
  } else if (crystal_val <= 8000000) {
    temp_reg |=
      (MCG_C2_RANGE0(1) | (hgo_val << MCG_C2_HGO0_SHIFT) |
       (erefs_val << MCG_C2_EREFS0_SHIFT));
  } else {
    temp_reg |=
      (MCG_C2_RANGE0(2) | (hgo_val << MCG_C2_HGO0_SHIFT) |
       (erefs_val << MCG_C2_EREFS0_SHIFT));
  }
  MCG->C2 = temp_reg;

  if (crystal_val <= 1250000) 
  {
    frdiv_val = 0;
  } 
  else if (crystal_val <= 2500000) 
  {
    frdiv_val = 1;
  } 
  else if (crystal_val <= 5000000) 
  {
    frdiv_val = 2;
  } 
  else if (crystal_val <= 10000000) 
  {
    frdiv_val = 3;
  } 
  else if (crystal_val <= 20000000) 
  {
    frdiv_val = 4;
  } 
  else 
  {
    frdiv_val = 5;
  }

  //
  // Select external oscilator and Reference Divider and clear IREFS
  // to start ext osc If IRCLK is required it must be enabled outside
  // of this driver, existing state will be maintained
  // CLKS=2, FRDIV=frdiv_val, IREFS=0, IRCLKEN=0, IREFSTEN=0
  //
  temp_reg = MCG->C1;
  // Clear values in these fields
  temp_reg &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK);
  // Set the required CLKS and FRDIV values
  temp_reg = MCG_C1_CLKS(2) | MCG_C1_FRDIV(frdiv_val);
  MCG->C1 = temp_reg;

  // if the external oscillator is used need to wait for OSCINIT to set
  if (erefs_val)
  {
    if (!mcg_wait_status<10000, MCG_S_OSCINIT0_MASK, MCG_S_OSCINIT0_SHIFT, 1>())
      return std::make_tuple(false, 0x23);
  }
  // wait for Reference clock Status bit to clear
  if (!mcg_wait_status<2000, MCG_S_IREFST_MASK, MCG_S_IREFST_SHIFT, 0>())
    return std::make_tuple(false, 0x11);

  // Wait for clock status bits to show clock source is ext ref clk
  if (!mcg_wait_status<2000, MCG_S_CLKST_MASK, MCG_S_CLKST_SHIFT, 0x02>())
    return std::make_tuple(false, 0x1A);

  //
  // Now in FBE
  // It is recommended that the clock monitor is enabled when using an
  // external clock as the clock source/reference. It is enabled here
  // but can be removed if this is not required.
  //
  MCG->C6 |= MCG_C6_CME0_MASK;

  return std::make_tuple(true, 0x00); // crystal_value
}


inline result_t fbe_pbe(unsigned    crystal_val,
                        signed char prdiv_val,
                        signed char vdiv_val)
{
  unsigned char temp_reg;
  //
  // Configure MCG->C5
  // If the PLL is to run in STOP mode then the PLLSTEN bit needs to be
  // OR'ed in here or in user code.
  //
  temp_reg = MCG->C5;
  temp_reg &= ~MCG_C5_PRDIV0_MASK;
  temp_reg |= MCG_C5_PRDIV0(prdiv_val - 1);  /*set PLL ref divider */
  MCG->C5 = temp_reg;

  //
  // Configure MCG->C6
  // The PLLS bit is set to enable the PLL, MCGOUT still sourced from
  // ext ref clk. The clock monitor is not enabled here as it has likely
  // been enabled previously and so the value of CME is not altered here.
  // The loss of lock interrupt can be enabled by seperately OR'ing in the
  // LOLIE bit in MCG->C6
  //
  temp_reg = MCG->C6;      /* store present C6 value */
  temp_reg &= ~MCG_C6_VDIV0_MASK;  /* clear VDIV settings */
  // write new VDIV and enable PLL
  temp_reg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(vdiv_val - 24);
  MCG->C6 = temp_reg;      /* update MCG->C6 */

  // wait for PLLST status bit to set
  if (!mcg_wait_status<2000, MCG_S_PLLST_MASK, MCG_S_PLLST_SHIFT, 1>())
    return std::make_tuple(false, 0x16);      /* check bit is really set and return with error if not set */

  // Wait for LOCK bit to set
  if (!mcg_wait_status<10000, MCG_S_LOCK0_MASK, MCG_S_LOCK0_SHIFT, 1>())
    return std::make_tuple(false, 0x44);      /* check bit is really set and return with error if not set */

  // now in PBE
  return std::make_tuple(true, 0x00);      /* MCGOUT frequency equals external clock frequency */
} // fbe_pbe

inline result_t
pbe_pee(unsigned crystal_val)
{
  // Select PLL as MCG_OUT
  MCG->C1 &= ~MCG_C1_CLKS_MASK;

  // Wait for clock status bits to update
  if (!mcg_wait_status<2000, MCG_S_CLKST_MASK, MCG_S_CLKST_SHIFT, 3>())
    return std::make_tuple(false, 0x1B);      // check CLKST is set correctly and return with error if not

  // Now in PEE
  return std::make_tuple(true, 0);  // MCGOUT equals PLL output frequency
}

template<unsigned CrystalFreq, 
         bool     UseCrystal,
         bool     HighGain,
         unsigned ClkDiv,
         unsigned ClkMult>
inline bool init_pll()
{
  check_crystal_freq<CrystalFreq, UseCrystal>();
  check_pll_parameters<CrystalFreq, ClkDiv, ClkMult>();

  result_t result = std::make_tuple(false, 0);

  if (is_in_fei_mode())
  {
    result = fei_fbe(CrystalFreq, UseCrystal, HighGain);
    if (std::get<0>(result))
    {
      result = fbe_pbe(CrystalFreq, ClkDiv, ClkMult);
      if (std::get<0>(result))
      {
        result = pbe_pee(CrystalFreq);
        if (std::get<0>(result))
        {
          SIM_SOPT2   |= SIM_SOPT2_PLLFLLSEL(1); // select PLL
        }
      }
    }
  }
  
  return std::get<0>(result);
}

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

} // namespace haluj

} // namespace base

} // namespace devices

} // namespace arm

} // namespace kinetis

// HALUJ_BASE_DEVICES_ARM_KINETIS_MCG_HPP
#endif
