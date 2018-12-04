/// \file tpm.hpp
/// TPM peripheral for cortex-m0+ kinetis devices
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

#ifndef HALUJ_BASE_DEVICES_KINETIS_TPM_HPP
#define HALUJ_BASE_DEVICES_KINETIS_TPM_HPP

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

template <unsigned Count,
          typename Predicate,
          typename Idle>
inline bool wait_if(Predicate p, Idle idle)
{
  for (unsigned i = 0; i < Count && p(); i++)
  {
    idle();
  }
  return p();
}

template <typename Specifier>
struct tpm : peripheral<Specifier>
{
  static constexpr TPM_Type* tpm_addr()  {return Specifier::tpm_addr;}

  template<std::size_t Channel>
  struct channel
  {
    typedef decltype(&TPM_Type::CONTROLS[Channel])  channel_type;
    
    static constexpr channel_type channel_addr() 
      {return &tpm<Specifier>::tpm_addr()->CONTROLS[Channel]; }
    
    template<uint32_t Value> 
    struct configuration_option
    {
      static constexpr uint32_t value    = Value;
      
      template<std::size_t T>
      uint32_t accept(channel<T>)
      {
        return  value;
      }
    };
    
    struct modes
    {
      static configuration_option<0b00000000> disable;
      static configuration_option<0b00010000> software_compare;
      static configuration_option<0b00000100> input_compare_re; // raising edge
      static configuration_option<0b00001000> input_compare_fe; // falling edge
      static configuration_option<0b00001100> input_compare_be; // rising or falling edges
      static configuration_option<0b00010100> output_compare_tom;
      static configuration_option<0b00011000> output_compare_com;
      static configuration_option<0b00011100> output_compare_som;
      static configuration_option<0b00101000> pwm_htp;
      static configuration_option<0b00100100> pwm_ltp;
      static configuration_option<0b00111000> output_compare_polm;
      static configuration_option<0b00110100> output_compare_pohm;
    };
    
    template<typename Options>
    static void configure(Options p_opts)
    {
      channel_addr()->CnSC = 
        p_opts.template accept<uint32_t>(channel<Channel>(), or_op());
    }

    static void set(unsigned p_value)
    {
      channel_addr()->CnV = p_value;
    }
    
    static void stop()
    {
      wait_if<1000>(
        [&]() -> bool 
          { 
            return (channel_addr()->CnSC & ~TPM_CnSC_CHF_MASK) != 0; 
          },
        [&]()
          { 
            channel_addr()->CnSC = TPM_CnSC_CHF_MASK;
            channel_addr()->CnSC = 0;
          }
      );
    }
  };
  
  typedef channel<0>    channel_0;
  typedef channel<1>    channel_1;
    
  template<uint32_t Value> 
  struct configuration_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(tpm<T>)
    {
      return  value;
    }
  };

  struct counter_modes
  {
    static configuration_option<0>                  edge;
    static configuration_option<TPM_SC_CPWMS_MASK>  center;
  };

  struct clock_modes
  {
    static configuration_option<TPM_SC_CMOD(0)>   disable;
    static configuration_option<TPM_SC_CMOD(1)>   internal;
    static configuration_option<TPM_SC_CMOD(2)>   external;
  };

  struct prescalers
  {
    static configuration_option<TPM_SC_PS(0)>    _1;
    static configuration_option<TPM_SC_PS(1)>    _2;
    static configuration_option<TPM_SC_PS(2)>    _4;
    static configuration_option<TPM_SC_PS(3)>    _8;
    static configuration_option<TPM_SC_PS(4)>    _16;
    static configuration_option<TPM_SC_PS(5)>    _32;
    static configuration_option<TPM_SC_PS(6)>    _64;
    static configuration_option<TPM_SC_PS(7)>    _128;
  };  
  
  static void start()
  {}
  
  static void stop()
  {
    wait_if<1000>(
      [&]() -> bool 
        { 
          return (tpm_addr()->SC & ~TPM_SC_TOF_MASK) != 0; 
        }, 
      [&]() {
        clear();
      }
    );
  }
  
  static void clear()
  {
    tpm_addr()->SC = TPM_SC_TOF_MASK;
    tpm_addr()->SC = 0;    
  }
  
  template<typename Options>
  static void configure(Options p_opts)
  {
    stop();
    tpm_addr()->SC = 
      p_opts.template accept<uint32_t>(tpm<Specifier>(), or_op());
    start();
  }  
  
  static bool is_open()
  {
    return (tpm_addr()->SC != 0); 
  }
  
  static void set_count(const unsigned p_value)
  {
    tpm_addr()->CNT = TPM_CNT_COUNT(p_value);
  }
  
  static unsigned get_count()
  {
    return tpm_addr()->CNT >> TPM_CNT_COUNT_SHIFT;
  }
  
  static void set_mod(const unsigned p_value)
  {
    tpm_addr()->MOD = TPM_MOD_MOD(p_value);
  }

};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_KINETIS_TPM_HPP
#endif
