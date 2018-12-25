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
  enum status_flags
  {
    rxak  = I2C_S_RXAK_SHIFT,
    iicif = I2C_S_IICIF_SHIFT,
    srw   = I2C_S_SRW_SHIFT,
    ram   = I2C_S_RAM_SHIFT,
    arbl  = I2C_S_ARBL_SHIFT,
    busy  = I2C_S_BUSY_SHIFT,
    iaas  = I2C_S_IAAS_SHIFT,
    tcf   = I2C_S_TCF_SHIFT
  };  
  
  
  static I2C_Type*  i2c_addr()        {return reinterpret_cast<I2C_Type*>(Specifier::i2c_base);}
  static constexpr bool       uses_core_clock() {return Specifier::uses_core_clock;}
  
  static constexpr void enable_ack()
  {
    i2c_addr()->C1 &= ~I2C_C1_TXAK_MASK;
  }

  static constexpr void disable_ack()
  {
    i2c_addr()->C1 |= I2C_C1_TXAK_MASK;
  }
 
  static constexpr void enable_tx()
  {
    i2c_addr()->C1 |= I2C_C1_TX_MASK;
  }

  static constexpr void disable_tx()
  {
    i2c_addr()->C1 &= ~I2C_C1_TX_MASK;
  }
  
  static constexpr bool is_tx_enabled()
  {
    return bit_test(i2c_addr()->C1, I2C_C1_TX_SHIFT);
  }  

  static constexpr void idle()
  {
    i2c_addr()->C1 = I2C_C1_IICEN_MASK;
  }

  static constexpr void repeated_start()
  {
    i2c_addr()->C1 |= I2C_C1_RSTA_MASK;
  }

  static constexpr void clear_iicif()
  {
    #if defined(K64F12) || defined(K60D10)
    i2c_addr()->S |= I2C_S_IICIF_MASK;
    #else
    i2c_addr()->S1 |= I2C_S_IICIF_MASK;
    #endif    
  }

  static constexpr void clear_arbl()
  {
    #if defined(K64F12) || defined(K60D10)
    i2c_addr()->S  |= I2C_S_ARBL_MASK;
    #else
    i2c_addr()->S1 |= I2C_S_ARBL_MASK;
    #endif
  }
  
  static constexpr bool test(const status_flags p_flags)
  {
    return bit_test(status(), p_flags);
  }

  // Configuration Options

  template<bool Value> 
  struct address
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
  struct baud_rate
  {
    static constexpr bool value    = Value;
    
    template<typename T>
    uint32_t accept(i2c<T>)
    {
      i2c<T>::i2c_addr()->F = /*p_mul |*/ I2C_F_ICR(value);
      return  0;
    }
  };
  
  // Static Methods
  
  static constexpr  uint32_t status()
  {
    #if defined(K64F12) || defined(K60D10)
    return i2c_addr()->S;
    #else
    return i2c_addr()->S1;
    #endif
  }

  static constexpr void start()
  {
    i2c_addr()->C1 |= I2C_C1_MST_MASK;
  }

  static constexpr void stop()
  {
    i2c_addr()->C1 &= ~I2C_C1_MST_MASK;
  }

  static constexpr void clear()
  {
    i2c_addr()->C1 = 0U;
    i2c_addr()->A1 = 0U;    
  }
  
  static constexpr uint8_t read()
  {
    return i2c_addr()->D;
  }
  
  static constexpr void write(const uint8_t p_value)
  {
    i2c_addr()->D = p_value;
  }  
  
  template<typename Options>
  static constexpr void configure(Options p_opts)
  {
    idle();
    p_opts.template accept<uint32_t>(i2c<Specifier>(), null_op());
  }
  
};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_ARM_KINETIS_I2C_HPP
#endif
