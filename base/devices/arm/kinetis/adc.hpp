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
#include "specific.hpp"

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
  
  static void start(const unsigned  p_reg, 
                    const unsigned  p_channel, 
                    const bool      p_is_differential = false)
  {
    specific::start(*adc_addr(), p_reg, p_channel, p_is_differential);
  }

  static void stop()
  {}
  
  static void clear()
  {}

  static void configure(); // <-- This function is temprorary
  
  static void calibrate();  
  
  static bool is_data_available(const unsigned p_reg)
  {
    return specific::is_data_available(*adc_addr(), p_reg);
  }

  static uint32_t read(const unsigned p_reg)
  {
    return specific::read(*adc_addr(), p_reg);
  }
  
};

template<typename T>
void adc<T>::configure()
{
  specific::configure(*adc_addr());
}  

template<typename T>
void adc<T>::calibrate()
{
  specific::calibrate(*adc_addr());  
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_KINETIS_ADC_HPP
#endif
