/// \file adc.hpp
/// ADC peripheral for kinetis devices
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

#ifndef HALUJ_BASE_DEVICES_KINETIS_ADC_HPP
#define HALUJ_BASE_DEVICES_KINETIS_ADC_HPP

#include "peripheral.hpp"
#include "options.hpp"
#include "bitops.hpp"

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

template <typename Specifier>
struct adc : peripheral<Specifier>
{
  static constexpr ADC_Type* adc_addr()  {return reinterpret_cast<ADC_Type*>(Specifier::adc_base);}

  template<std::size_t Channel>
  struct channel
  {};
  
  static void start(const unsigned  channel, 
                    const bool      is_differential = false)
  {
    #ifdef ADC_SC1_DIFF_MASK
    if (is_differential)
    {
      adc_addr()->SC1[0] = ADC_SC1_DIFF_MASK | ADC_SC1_ADCH(channel);
    }
    else
    {
      adc_addr()->SC1[0] = ADC_SC1_ADCH(channel);
    }    
    #else
      adc_addr()->SC1[0] = ADC_SC1_ADCH(channel);
    #endif
  }

  static void stop()
  {}
  
  static void clear()
  {}

  /*
  template<typename Options>
  static void configure(Options p_opts)
  {
    
    
  }  */
  
  static void configure()
  {
    adc_addr()->CFG1 =  ADC_CFG1_ADIV(3)     | // 2^3=8 /8
                    ADC_CFG1_ADLSMP_MASK |
                    ADC_CFG1_MODE(0x03)  |  // 16-Bit Mode
                    ADC_CFG1_ADICLK(0);   // Bus Clock

    adc_addr()->CFG2 = // ADC_CFG2_MUXSEL_MASK | // Enable for SE_b inputs
                   ADC_CFG2_ADLSTS(0x00); // ADLSTS_20

    adc_addr()->CV1  = 0x1234u;
    adc_addr()->CV2  = 0x5678u;
    adc_addr()->SC2  = ADC_SC2_REFSEL(0x00); // REFSEL_EXT (VREFH ve VREFL)

    adc_addr()->SC3  = ADC_SC3_AVGE_MASK       |
                   ADC_SC3_AVGS(0x00); // 32 Sample Average

    #if defined(K60D10)
    adc_addr()->PGA  = ADC_PGA_PGAG(0); // Gain 2^0
    #endif
    
    // set_gain(device_, 0); // define in common.h as inline 
  }  
  
  static void calibrate()
  {
    unsigned short cal_var;

    adc_addr()->SC2 &= ~ADC_SC2_ADTRG_MASK; // Enable Software Conversion Trigger for Calibration Process    - adc_addr()->SC2 = adc_addr()->SC2 | ADC_SC2_ADTRGW(0);
    adc_addr()->SC3 &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); // set single conversion, clear avgs bitfield for next writing
    adc_addr()->SC3 |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(0x03) );  // Turn averaging ON and set at max value ( 32 )

    adc_addr()->SC3 |= ADC_SC3_CAL_MASK ;      // Start CAL
    while ( (adc_addr()->SC1[0] & ADC_SC1_COCO_MASK ) == 0 ); // Wait calibration end

    if ((adc_addr()->SC3 & ADC_SC3_CALF_MASK) == ADC_SC3_CALF_MASK )
    {
      return;    // Check for Calibration fail error and return
    }
    // Calculate plus-side calibration
    cal_var = 0x00;

    cal_var =  adc_addr()->CLP0;
    cal_var += adc_addr()->CLP1;
    cal_var += adc_addr()->CLP2;
    cal_var += adc_addr()->CLP3;
    cal_var += adc_addr()->CLP4;
    cal_var += adc_addr()->CLPS;

    cal_var = cal_var/2;
    cal_var |= 0x8000; // Set MSB

    adc_addr()->PG = ADC_PG_PG(cal_var);

    // Calculate minus-side calibration
    cal_var = 0x00;

    cal_var =  adc_addr()->CLM0;
    cal_var += adc_addr()->CLM1;
    cal_var += adc_addr()->CLM2;
    cal_var += adc_addr()->CLM3;
    cal_var += adc_addr()->CLM4;
    cal_var += adc_addr()->CLMS;

    cal_var = cal_var/2;

    cal_var |= 0x8000; // Set MSB

    #ifdef ADC_MG_MG
    adc_addr()->MG = ADC_MG_MG(cal_var);
    #else 
    #warning "Skipping adc_addr()->MG = ADC_MG_MG(cal_var)"
    #endif

    adc_addr()->SC3 &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */
  }  
  
  static bool is_data_available(const unsigned offset)
  {
    return (( adc_addr()->SC1[offset] & ADC_SC1_COCO_MASK ) == ADC_SC1_COCO_MASK);
  }

  static uint32_t read(const unsigned offset)
  {
    return adc_addr()->R[offset];
  }
  
};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_KINETIS_ADC_HPP
#endif
