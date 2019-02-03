/// \file uart.hpp
/// generic uart implementation for Kinetis family devices
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_UART_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_UART_HPP

#include "peripheral.hpp"
#include "options.hpp"
#include "core.hpp"
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
struct uart : peripheral<Specifier>
{
  static UART_Type*     uart_addr() {return reinterpret_cast<UART_Type*>(Specifier::uart_base);}
  
  static constexpr bool uses_core_clock() {return Specifier::uses_core_clock;}

  template<uint32_t Value> 
  struct baud_rate
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(uart<T>)
    {
      uint32_t peripheral_clock = system_core_clock();
      /*static*/ if (!uart<T>::uses_core_clock())
      {
        peripheral_clock = system_bus_clock();
      }
      uint32_t sbr = specific::set_baud_rate(*uart<T>::uart_addr(),  peripheral_clock, value);
      (void) specific::set_fractional_divider(*uart<T>::uart_addr(), peripheral_clock, value, sbr);
      return  0;
    }
  };

  template<uint32_t Value> 
  struct bits_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(uart<T>)
    {
      specific::set_bits(*uart<T>::uart_addr(), value);
      return  0;
    }
  };

  struct bits
  {
    static bits_option<8>   _8;
    static bits_option<9>   _9;
    static bits_option<10>  _10;
  };

  template<uint32_t Value> 
  struct parity_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(uart<T>)
    {
      specific::set_parity(*uart<T>::uart_addr(), value);
      return  0;
    }
  };

  struct parity
  {
    static parity_option<0>   none;
    static parity_option<1>   even;
    static parity_option<2>   odd ;
  };

  template<uint32_t Value> 
  struct stop_bits_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(uart<T>)
    {
      specific::set_stop_bits(*uart<T>::uart_addr(), value);
      return  0;
    }
  };

  struct stop_bits
  {
    static stop_bits_option<0>    one;
    static stop_bits_option<1>    two;
  };
  
  // Static Methods  
  
  static constexpr uint32_t read() 
  {
    return specific::read(*uart_addr());
  }
  
  static constexpr bool is_rx_available() 
  {
    return specific::is_rx_available(*uart_addr());
  }

  static constexpr bool is_error() 
  {
    return specific::is_error(*uart_addr());
  }
  
  static constexpr void write(const uint32_t p_data) 
  {
    specific::write(*uart_addr(), p_data);
  }

  static constexpr bool is_tx_ready()
  {
    return specific::is_tx_ready(*uart_addr());
  }  

  static constexpr void start()
  {
    specific::start(*uart_addr());
  }

  static constexpr void stop()
  {
    specific::stop(*uart_addr());
  }

  template<typename Options>
  static constexpr void configure(Options p_opts)
  {
    stop();
    specific::clear(*uart_addr());
    p_opts.template accept<uint32_t>(uart<Specifier>(), null_op());
    start();
  }

};

// ugly and cumbersome instantiation of static members.
// A compiler with good optimizer is expected to get rid off them
// So it may not complain even if they are missing.
// However it is defined to conform the standart
template<typename T> typename uart<T>::template bits_option<8>      uart<T>::bits::_8;
template<typename T> typename uart<T>::template bits_option<9>      uart<T>::bits::_9;
template<typename T> typename uart<T>::template bits_option<10>     uart<T>::bits::_10;

template<typename T> typename uart<T>::template parity_option<0>    uart<T>::parity::none;
template<typename T> typename uart<T>::template parity_option<1>    uart<T>::parity::even;
template<typename T> typename uart<T>::template parity_option<2>    uart<T>::parity::odd;

template<typename T> typename uart<T>::template stop_bits_option<0> uart<T>::stop_bits::one;
template<typename T> typename uart<T>::template stop_bits_option<1> uart<T>::stop_bits::two;

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj


// HALUJ_BASE_DEVICES_ARM_KINETIS_UART_HPP
#endif

