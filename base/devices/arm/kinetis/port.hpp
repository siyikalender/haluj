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

template<typename T>
inline void configure(port<T>         p_port,
                      const unsigned  p_index,
                      const uint32_t  p_opt_val)
{
  port<T>::port_addr()->PCR[p_index] = p_opt_val;
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
inline void set_direction(port<T>, const uint32_t p_dir)
{
  port<T>::gpio_addr()->PDDR = p_dir;
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

// options for port configuration

struct pull
{
  static constexpr uint32_t none    = 0;
  static constexpr uint32_t down    = 2;
  static constexpr uint32_t up      = 3;
};

// Note: slew_rate may not be supported by all devices
struct slew_rate
{
  static constexpr uint32_t fast    = 0;
  static constexpr uint32_t slow    = PORT_PCR_SRE_MASK;
};

// Note: drive_strength may not be supported by all devices
struct drive_strength
{
  static constexpr uint32_t low     = 0;
  static constexpr uint32_t high    = PORT_PCR_DSE_MASK;
};

// Note: open_drain may not be supported by all devices
struct open_drain
{
  static constexpr uint32_t disable = 0;
  static constexpr uint32_t enable  = PORT_PCR_ODE_MASK;
};

struct interrupts
{
  static constexpr uint32_t none            = PORT_PCR_IRQC(0);
  static constexpr uint32_t on_logic_0      = PORT_PCR_IRQC(8);
  static constexpr uint32_t on_rising_edge  = PORT_PCR_IRQC(9);
  static constexpr uint32_t on_falling_edge = PORT_PCR_IRQC(10);
  static constexpr uint32_t on_both_edges   = PORT_PCR_IRQC(11);
  static constexpr uint32_t on_logic_1      = PORT_PCR_IRQC(12);
};

// Note: passive_filter may not be supported by all devices
struct passive_filter
{
  static constexpr uint32_t disable = 0;
  static constexpr uint32_t enable  = PORT_PCR_PFE_MASK;  
};

struct mux
{
  static constexpr uint32_t _0 = PORT_PCR_MUX(0);
  static constexpr uint32_t _1 = PORT_PCR_MUX(1);  
  static constexpr uint32_t _2 = PORT_PCR_MUX(2);  
  static constexpr uint32_t _3 = PORT_PCR_MUX(3);  
  static constexpr uint32_t _4 = PORT_PCR_MUX(4);  
  static constexpr uint32_t _5 = PORT_PCR_MUX(5);  
  static constexpr uint32_t _6 = PORT_PCR_MUX(6);  
  static constexpr uint32_t _7 = PORT_PCR_MUX(7);  
};

constexpr uint32_t options(const uint32_t p_value)
{
  return p_value;
}

template<typename... Args>
constexpr uint32_t options(const uint32_t p_value, Args... args)
{
  return p_value | options(args...);
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// _HALUJ_BASE_DEVICES_KINETIS_PORT_HPP
#endif

