/// \file i2c.hpp
/// generic i2c definitions for Kinetis family devices
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_I2C_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_I2C_HPP

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
struct i2c : peripheral<Specifier>
{
  static constexpr I2C_Type*  i2c_addr()        {return Specifier::i2c_addr;}
  static constexpr bool       uses_core_clock() {return Specifier::uses_core_clock;}
};
  // Configuration Options
  template<bool Value> 
  struct i2c_address
  {
    static constexpr bool value    = Value;
    
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      if (value > 0)
      {
        i2c<T>::i2c_addr()->A1 = (value << 1);
      }
      return  0;
    }
  };

  template<bool Value> 
  struct i2c_baud_rate
  {
    static constexpr bool value    = Value;
    
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      i2c<T>::i2c_addr()->F = /*p_mul |*/ I2C_F_ICR(value);
      return  0;
    }
  };

  // Control Options

  template<bool Value> 
  struct i2c_ack_option
  {
    static constexpr bool value    = Value;
    
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      /*static*/ if (value)
        i2c<T>::i2c_addr()->C1 &= ~I2C_C1_TXAK_MASK;
      else
        i2c<T>::i2c_addr()->C1 |= I2C_C1_TXAK_MASK;
      return  0;
    }
  };

  template<bool Value> 
  struct i2c_tx_option
  {
    static constexpr bool value    = Value;
    
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      /*static*/ if (value)
        i2c<T>::i2c_addr()->C1 |= I2C_C1_TX_MASK;
      else
        i2c<T>::i2c_addr()->C1 &= ~I2C_C1_TX_MASK;
      return  0;
    }
  };

  struct i2c_idle_option
  {
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      i2c<T>::i2c_addr()->C1 = I2C_C1_IICEN_MASK;
      return  0;
    }
  };

  struct i2c_repeated_start_option
  {
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      // repeated_start(*i2c<T>::i2c_addr());
      i2c<T>::i2c_addr()->C1 |= I2C_C1_RSTA_MASK;
      return  0;
    }
  };

  struct i2c_clear_iicif_option
  {
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      #if defined(K64F12) || defined(K60D10)
      i2c<T>::i2c_addr()->S |= I2C_S_IICIF_MASK;
      #else
      i2c<T>::i2c_addr()->S1 |= I2C_S_IICIF_MASK;
      #endif
      return  0;
    }
  };

  struct i2c_clear_arbl_option
  {
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      #if defined(K64F12) || defined(K60D10)
      i2c<T>::i2c_addr()->S  |= I2C_S_ARBL_MASK;
      #else
      i2c<T>::i2c_addr()->S1 |= I2C_S_ARBL_MASK;
      #endif
      return  0;
    }
  };
 
struct i2c_control_options
{
  static i2c_ack_option<false>       disable_ack;
  static i2c_ack_option<true>        enable_ack;
  static i2c_tx_option<false>        disable_tx;
  static i2c_tx_option<true>         enable_tx;
  static i2c_idle_option             idle;
  static i2c_repeated_start_option   repeated_start;
  static i2c_clear_iicif_option      clear_iicf;
  static i2c_clear_arbl_option       clear_arbl;
};

////////////////////////////////////////

template<typename T, typename Options>
inline void control(i2c<T> p_i2c, Options p_opts)
{
  (void) p_opts.template accept<uint32_t>(p_i2c, null_op());  
}

template<typename T, typename Options>
inline uint32_t status(i2c<T> p_i2c, Options p_opts)
{
  #if defined(K64F12) || defined(K60D10)
  return i2c<T>::i2c_addr()->S;
  #else
  return i2c<T>::i2c_addr()->S1;
  #endif
}

template<typename T>
inline void start(i2c<T>)
{
  i2c<T>::i2c_addr()->C1 |= I2C_C1_MST_MASK;
}

template<typename T>
inline void stop(i2c<T>)
{
  i2c<T>::i2c_addr()->C1 &= ~I2C_C1_MST_MASK;
}

template<typename T>
inline void clear(i2c<T>)
{
  i2c<T>::i2c_addr()->C1 = 0U;
  i2c<T>::i2c_addr()->A1 = 0U;    
}

template<typename T, typename Options>
inline void configure(i2c<T> p_i2c, Options p_opts)
{
  control(p_i2c, options(i2c_control_options::idle));

  p_opts.template accept<uint32_t>(p_i2c, null_op());
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_I2C_HPP
#endif
