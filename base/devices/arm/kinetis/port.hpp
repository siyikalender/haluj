/// \file port.hpp
/// generic port definitions for Kinetis family devices
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

#ifndef HALUJ_BASE_DEVICES_KINETIS_PORT_HPP
#define HALUJ_BASE_DEVICES_KINETIS_PORT_HPP

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
struct port : peripheral<Specifier>
{
  static PORT_Type* port_addr() { return reinterpret_cast<PORT_Type*>(Specifier::port_base); }
  
  static GPIO_Type* gpio_addr() { return reinterpret_cast<GPIO_Type*>(Specifier::gpio_base); }  
  
  // options for port configuration
  template<uint32_t Value> 
  struct port_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    static constexpr uint32_t accept(port<T>)
    {
      return value;
    }
  };

  struct pull
  {
    typedef port_option<0> none;
    typedef port_option<2> down;
    typedef port_option<3> up;
  };

  // Note: slew_rate may not be supported by all devices
  struct slew_rate
  {
    typedef port_option<0>                  fast;
    typedef port_option<PORT_PCR_SRE_MASK>  slow;
  };

  // Note: drive_strength may not be supported by all devices
  struct drive_strength
  {
    typedef port_option<0>                  low;
    typedef port_option<PORT_PCR_DSE_MASK>  high;
  };

  // Note: open_drain may not be supported by all devices
  struct open_drain
  {
    typedef port_option<0>                  disable;
    typedef port_option<PORT_PCR_ODE_MASK>  enable;
  };

  struct interrupts
  {
    typedef port_option<PORT_PCR_IRQC(0)>  none;
    typedef port_option<PORT_PCR_IRQC(8)>  on_logic_0;
    typedef port_option<PORT_PCR_IRQC(9)>  on_rising_edge;
    typedef port_option<PORT_PCR_IRQC(10)> on_falling_edge;
    typedef port_option<PORT_PCR_IRQC(11)> on_both_edges;
    typedef port_option<PORT_PCR_IRQC(12)> on_logic_1;
  };

  // Note: passive_filter may not be supported by all devices
  struct passive_filter
  {
    typedef port_option<0>                   disable;
    typedef port_option<PORT_PCR_PFE_MASK>   enable;
  };

  struct mux
  {
    typedef port_option<PORT_PCR_MUX(0)> _0;
    typedef port_option<PORT_PCR_MUX(1)> _1;  
    typedef port_option<PORT_PCR_MUX(2)> _2;  
    typedef port_option<PORT_PCR_MUX(3)> _3;  
    typedef port_option<PORT_PCR_MUX(4)> _4;  
    typedef port_option<PORT_PCR_MUX(5)> _5;  
    typedef port_option<PORT_PCR_MUX(6)> _6;  
    typedef port_option<PORT_PCR_MUX(7)> _7;  
  };

  enum class directions
  {
    input,
    output,
    bidirectional
  };

  template<typename Options>
  static void configure(const unsigned  p_index,
                                  Options         p_opts)
  {
    port_addr()->PCR[p_index] = 
      p_opts.template accept<uint32_t>(port<Specifier>(), or_op());
  }

  static void set_direction(const uint32_t p_value)
  {
    gpio_addr()->PDDR = mask_set(gpio_addr()->PDDR, p_value);
  }

  static void clear_direction(const uint32_t p_value)
  {
    gpio_addr()->PDDR = mask_clear(gpio_addr()->PDDR, p_value);
  }
  
  static constexpr void set(const uint32_t p_value)
  {
    gpio_addr()->PSOR = p_value;
  }

  static void clear(const uint32_t p_value)
  {
    gpio_addr()->PCOR = p_value;
  }

  static void toggle(const uint32_t p_value)
  {
    gpio_addr()->PTOR = p_value;
  }

  static uint32_t read()
  {
    return gpio_addr()->PDIR;
  }

  static uint32_t read_out()
  {
    return gpio_addr()->PDOR;
  }
  
  static void write(const uint32_t p_value)
  {
    gpio_addr()->PDOR = p_value;
  }

  static bool test_interrupt_clear(const uint32_t p_index)
  {
    bool result     =  false;
    uint32_t value  = port_addr()->PCR[p_index];
    if ((result = bit_test(value, PORT_PCR_ISF_SHIFT)))
    {
      port_addr()->PCR[p_index] = value | PORT_PCR_ISF_SHIFT;
    }
    return result;
  }

};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_KINETIS_PORT_HPP
#endif

