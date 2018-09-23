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

#ifndef _HALUJ_BASE_DEVICES_KINETIS_PORT_HPP
#define _HALUJ_BASE_DEVICES_KINETIS_PORT_HPP

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
  static constexpr PORT_Type*  port_addr() {return Specifier::port_addr;}
  static constexpr GPIO_Type*  gpio_addr() {return Specifier::gpio_addr;}
};

enum class directions
{
  input,
  output,
  bidirectional
};

template<typename T, typename Options>
inline void configure(port<T>         p_port,
                      const unsigned  p_index,
                      Options         p_opts)
{
  port<T>::port_addr()->PCR[p_index] = 
    p_opts.template accept<uint32_t>(p_port, or_op());
}

template<typename T>
inline void set_direction(port<T>, 
                          const unsigned    p_index, 
                          const directions  p_dir)
{
  T::gpio_addr->PDDR = 
    bit_assign(T::gpio_addr->PDDR, p_index, (p_dir == directions::output));
}

template<typename T>
inline void set_direction(port<T>, const uint32_t p_value)
{
  port<T>::gpio_addr()->PDDR = p_value;
}

template<typename T>
inline void set(port<T>, const uint32_t p_value)
{
  port<T>::gpio_addr()->PSOR = p_value;
}

template<typename T>
inline void clear(port<T>, const uint32_t p_value)
{
  port<T>::gpio_addr()->PCOR = p_value;
}

template<typename T>
inline void toggle(port<T>, const uint32_t p_value)
{
  port<T>::gpio_addr()->PTOR = p_value;
}

template<typename T>
inline uint32_t read(port<T>)
{
  return port<T>::gpio_addr()->PDIR;
}

template<typename T>
inline void write(port<T>, const uint32_t p_value)
{
  port<T>::gpio_addr()->PDOR = p_value;
}

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
  static port_option<0> none;
  static port_option<2> down;
  static port_option<3> up;
};

// Note: slew_rate may not be supported by all devices
struct slew_rate
{
  static port_option<0>                  fast;
  static port_option<PORT_PCR_SRE_MASK>  slow;
};

// Note: drive_strength may not be supported by all devices
struct drive_strength
{
  static port_option<0>                  low;
  static port_option<PORT_PCR_DSE_MASK>  high;
};

// Note: open_drain may not be supported by all devices
struct open_drain
{
  static port_option<0>                  disable;
  static port_option<PORT_PCR_ODE_MASK>  enable;
};

struct interrupts
{
  static port_option<PORT_PCR_IRQC(0)>  none;
  static port_option<PORT_PCR_IRQC(8)>  on_logic_0;
  static port_option<PORT_PCR_IRQC(9)>  on_rising_edge;
  static port_option<PORT_PCR_IRQC(10)> on_falling_edge;
  static port_option<PORT_PCR_IRQC(11)> on_both_edges;
  static port_option<PORT_PCR_IRQC(12)> on_logic_1;
};

// Note: passive_filter may not be supported by all devices
struct passive_filter
{
  static port_option<0>                   disable;
  static port_option<PORT_PCR_PFE_MASK>   enable;
};

struct mux
{
  static port_option<PORT_PCR_MUX(0)> _0;
  static port_option<PORT_PCR_MUX(1)> _1;  
  static port_option<PORT_PCR_MUX(2)> _2;  
  static port_option<PORT_PCR_MUX(3)> _3;  
  static port_option<PORT_PCR_MUX(4)> _4;  
  static port_option<PORT_PCR_MUX(5)> _5;  
  static port_option<PORT_PCR_MUX(6)> _6;  
  static port_option<PORT_PCR_MUX(7)> _7;  
};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// _HALUJ_BASE_DEVICES_KINETIS_PORT_HPP
#endif

