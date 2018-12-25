/// \file vref.hpp
/// VREF peripheral for kinetis devices. VREF is not supported by
/// some kinetis devices. Please, check the reference manual
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

#ifndef HALUJ_BASE_DEVICES_KINETIS_VREF_HPP
#define HALUJ_BASE_DEVICES_KINETIS_VREF_HPP

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
struct vref : peripheral<Specifier>
{
  static constexpr VREF_Type* vref_addr()  {return reinterpret_cast<VREF_Type*>(Specifier::vref_base);}
  
  template<uint32_t Value> 
  struct configuration_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(vref<T>)
    {
      return value;
    }
  };  
  
  struct regulator
  {
    static configuration_option<0>                    disable;
    static configuration_option<VREF_SC_REGEN_MASK>   enable;
  };

  struct internal_voltage_reference
  {
    static configuration_option<0>                    disable;
    static configuration_option<VREF_SC_VREFEN_MASK>  enable;
  };

  struct compensation
  {
    static configuration_option<0>                    disable;
    static configuration_option<VREF_SC_ICOMPEN_MASK> enable;
  };
  
  struct modes
  {
    static configuration_option<VREF_SC_MODE_LV(0)>  stand_by;
    static configuration_option<VREF_SC_MODE_LV(1)>  high_power_out;
    static configuration_option<VREF_SC_MODE_LV(2)>  low_power_out;
  };
  
  template<typename Options>
  static void configure(Options p_opts)
  {
    vref_addr()->SC =
      p_opts.template accept<uint32_t>(vref<Specifier>(), or_op());
  }

  static bool is_stable()
  {
    return bit_test(vref_addr()->SC, VREF_SC_VREFST_SHIFT);
  }
};

// ugly and cumbersome instantiation of static members.
// A compiler with good optimizer is expected to get rid off them
// So it may not complain even if they are missing.
// However it is defined to conform the standart
template<typename T> typename vref<T>::template configuration_option<0>                    vref<T>::regulator::disable;
template<typename T> typename vref<T>::template configuration_option<VREF_SC_REGEN_MASK>   vref<T>::regulator::enable;

template<typename T> typename vref<T>::template configuration_option<0>                    vref<T>::internal_voltage_reference::disable;
template<typename T> typename vref<T>::template configuration_option<VREF_SC_VREFEN_MASK>  vref<T>::internal_voltage_reference::enable;

template<typename T> typename vref<T>::template configuration_option<0>                    vref<T>::compensation::disable;
template<typename T> typename vref<T>::template configuration_option<VREF_SC_ICOMPEN_MASK> vref<T>::compensation::enable;
  
template<typename T> typename vref<T>::template configuration_option<VREF_SC_MODE_LV(0)>   vref<T>::modes::stand_by;
template<typename T> typename vref<T>::template configuration_option<VREF_SC_MODE_LV(1)>   vref<T>::modes::high_power_out;
template<typename T> typename vref<T>::template configuration_option<VREF_SC_MODE_LV(2)>   vref<T>::modes::low_power_out;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_KINETIS_VREF_HPP
#endif
