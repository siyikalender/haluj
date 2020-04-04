/// \file ftm.hpp
/// FTM peripheral for kinetis devices
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

#ifndef HALUJ_BASE_DEVICES_KINETIS_FTM_HPP
#define HALUJ_BASE_DEVICES_KINETIS_FTM_HPP

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
struct ftm : peripheral<Specifier>
{
  static constexpr FTM_Type* ftm_addr()  {return reinterpret_cast<FTM_Type*>(Specifier::ftm_base);}

  template<std::size_t Channel>
  struct channel
  {
    typedef decltype(&FTM_Type::CONTROLS[Channel])  channel_type;
    
    static constexpr channel_type channel_addr() 
      {return &ftm<Specifier>::ftm_addr()->CONTROLS[Channel]; }
    
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
    
    struct interrupt
    {
      typedef configuration_option<0>                    disabled;
      typedef configuration_option<FTM_CnSC_CHIE_MASK>   enable;
    };
    
    struct modes
    {
      // check all values
      typedef configuration_option<0b000000000> disable;
      typedef configuration_option<0b000000100> input_compare_re; // raising edge
      typedef configuration_option<0b000001000> input_compare_fe; // falling edge
      typedef configuration_option<0b000001100> input_compare_be; // rising or falling edges
      typedef configuration_option<0b000010100> output_compare_tom;
      typedef configuration_option<0b000011000> output_compare_com;
      typedef configuration_option<0b000011100> output_compare_som;
      typedef configuration_option<0b000101000> edge_pwm_htp;
      typedef configuration_option<0b000100100> edge_pwm_ltp;
      typedef configuration_option<0b000001000> center_pwm_htp;
      typedef configuration_option<0b000000100> center_pwm_ltp;
      typedef configuration_option<0b010001000> combine_pwm_htp;
      typedef configuration_option<0b010000100> combine_pwm_ltp;
    };
    
    template<typename Options>
    static inline void configure(Options p_opts)
    {
      channel_addr()->CnSC = 
        p_opts.template accept<uint32_t>(channel<Channel>(), or_op());
    }

    static inline void stop()
    {
      wait_if<1000>(
        [&]() -> bool 
          { 
            return (channel_addr()->CnSC & ~FTM_CnSC_CHF_MASK) != 0; 
          },
        [&]()
          { 
            channel_addr()->CnSC = FTM_CnSC_CHF_MASK;
            channel_addr()->CnSC = 0;
          }
      );
    }

    static inline void set(unsigned p_value)
    {
      channel_addr()->CnV = p_value;
    }
    
    static inline bool is_active()
    {
      uint32_t m = 
        mask(FTM_CnSC_ELSA_MASK | 
             FTM_CnSC_ELSB_MASK | 
             // FTM_CnSC_MSA_MASK |  do not care
             FTM_CnSC_MSB_MASK);

      return (channel_addr()->CnSC & m) == 0;
    }
      
    static inline bool test_interrupt()
    {
      return mask_test(channel_addr()->CnSC, FTM_CnSC_CHF_MASK);
    }

    static inline void clear_interrupt()
    {
      channel_addr()->CnSC |= FTM_CnSC_CHF_MASK;
    }    
    
  };
  
  typedef channel<0>    channel_0;
  typedef channel<1>    channel_1;
  typedef channel<2>    channel_2;
  typedef channel<3>    channel_3;
  typedef channel<4>    channel_4;
  typedef channel<5>    channel_5;
  typedef channel<6>    channel_6;
  typedef channel<7>    channel_7;
    
  template<uint32_t Value> 
  struct configuration_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(ftm<T>)
    {
      return  value;
    }
  };

  struct interrupt
  {
    typedef configuration_option<0>                  disabled;
    typedef configuration_option<FTM_SC_TOIE_MASK>   enable;
  };

  struct counter_modes
  {
    typedef configuration_option<0>                  edge;
    typedef configuration_option<FTM_SC_CPWMS_MASK>  center;
  };

  struct clock_modes
  {
    typedef configuration_option<FTM_SC_CLKS(0)>   disable;
    typedef configuration_option<FTM_SC_CLKS(1)>   internal;
    typedef configuration_option<FTM_SC_CLKS(2)>   fixed;
    typedef configuration_option<FTM_SC_CLKS(3)>   external;
  };

  struct prescalers
  {
    typedef configuration_option<FTM_SC_PS(0)>    _1;
    typedef configuration_option<FTM_SC_PS(1)>    _2;
    typedef configuration_option<FTM_SC_PS(2)>    _4;
    typedef configuration_option<FTM_SC_PS(3)>    _8;
    typedef configuration_option<FTM_SC_PS(4)>    _16;
    typedef configuration_option<FTM_SC_PS(5)>    _32;
    typedef configuration_option<FTM_SC_PS(6)>    _64;
    typedef configuration_option<FTM_SC_PS(7)>    _128;
  };  
  
  static void start()
  {
    // Enable clock
    ftm_addr()->SC |= FTM_SC_CLKS(1);
    // Enable write protection
    ftm_addr()->MODE &= ~FTM_MODE_WPDIS_MASK;
    // Reset Initial Count Value
    ftm_addr()->CNTIN = 0;
  }
  
  static inline void stop()
  {
    // Disable write protection
    ftm_addr()->MODE |= FTM_MODE_WPDIS_MASK;
    wait_if<1000>(
      [&]() -> bool 
        { 
          return (ftm_addr()->SC & ~FTM_SC_TOF_MASK) != 0; 
        }, 
      [&]() {
        clear();
      }
    );
    
    ftm_addr()->SC      &= ~FTM_SC_CLKS_MASK;
    ftm_addr()->QDCTRL  &= ~FTM_QDCTRL_QUADEN_MASK;
    set_count(0);
    set_mod(10000);
    ftm_addr()->CNTIN   = 0;
  }
  
  static inline void clear()
  {
    clear_interrupt();
    ftm_addr()->SC = 0;    
  }

  static inline  bool test_interrupt()
  {
    return bit_test(ftm_addr()->SC, FTM_SC_TOF_SHIFT);
  }

  static inline  void clear_interrupt()
  {
    ftm_addr()->SC |= FTM_SC_TOF_MASK;
  }

  static inline void clear_all_interrupts()
  {
    ftm_addr()->STATUS = FTM_STATUS_CH0F_MASK |   
                         FTM_STATUS_CH1F_MASK;
  }
  
  template<typename Options>
  static inline void configure(Options p_opts)
  {
    stop();
    ftm_addr()->SC = 
      p_opts.template accept<uint32_t>(ftm<Specifier>(), or_op());
    start();
  }  
  
  static inline bool is_running()
  {
    return (ftm_addr()->SC & FTM_SC_CLKS_MASK) != 0; 
  }
  
  static inline void set_count(const unsigned p_value)
  {
    ftm_addr()->CNT = FTM_CNT_COUNT(p_value);
  }

  static inline void set_initial(const unsigned p_value)
  {
    ftm_addr()->CNTIN = FTM_CNTIN_INIT(p_value);
  }
    
  static inline unsigned get_count()
  {
    return ftm_addr()->CNT >> FTM_CNT_COUNT_SHIFT;
  }
  
  static inline void set_mod(const unsigned p_value)
  {
    ftm_addr()->MOD = FTM_MOD_MOD(p_value);
  }

  static inline uint32_t get_mod()
  {
    return (ftm_addr()->MOD & FTM_MOD_MOD_MASK) >> FTM_MOD_MOD_SHIFT;
  }

};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_KINETIS_FTM_HPP
#endif
