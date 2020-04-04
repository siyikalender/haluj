/// \file spi.hpp
/// generic spi definitions for Kinetis family devices
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

#ifndef HALUJ_BASE_DEVICES_ARM_KINETIS_SPI_HPP
#define HALUJ_BASE_DEVICES_ARM_KINETIS_SPI_HPP

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
struct spi : peripheral<Specifier>
{
  static constexpr SPI_Type*  spi_addr()        {return reinterpret_cast<SPI_Type*>(Specifier::spi_base);}
  static constexpr bool       uses_core_clock() {return Specifier::uses_core_clock;}

  static constexpr void
  set_mode(const bool p_master)
  {
    if (p_master)
    {
      spi_addr()->MCR |= SPI_MCR_MSTR_MASK;
    }
    
    // initialize fifo
    spi_addr()->MCR &= ~SPI_MCR_MDIS_MASK;

    spi_addr()->MCR |= SPI_MCR_DIS_RXF_MASK |
                    SPI_MCR_DIS_TXF_MASK |
                    SPI_MCR_CLR_RXF_MASK |
                    SPI_MCR_CLR_TXF_MASK;
  }

  static constexpr void 
  set_active_cs(const uint32_t  p_active_cs_mask)
  {
    spi_addr()->MCR |= SPI_MCR_PCSIS(p_active_cs_mask);
  }

  static constexpr void
  set_frame_size(const uint32_t  p_frame_size)
  {
    spi_addr()->CTAR[0] |= SPI_CTAR_FMSZ(p_frame_size);
  }

  static constexpr void
  set_phase(const bool      p_phase)
  {
    if (p_phase)
      spi_addr()->CTAR[0] |= SPI_CTAR_CPHA_MASK;
  }

  template<bool Value> 
  struct mode_option
  {
    static constexpr bool value    = Value;
    
    template<typename T>
    uint32_t accept(spi<T>)
    {
      set_mode(value);
      return 0;
    }
  };

  template<uint32_t Value> 
  struct active_cs
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(spi<T>)
    {
      set_active_cs(value);
      return  0;
    }
  };

  template<uint32_t Value> 
  struct frame_size_option
  {
    static constexpr uint32_t value    = Value;
    
    template<typename T>
    uint32_t accept(spi<T>)
    {
      set_frame_size(value);
      return  0;
    }
  };

  template<bool Value> 
  struct phase_option
  {
    static constexpr bool value    = Value;
    
    template<typename T>
    uint32_t accept(spi<T>)
    {
      set_phase(value);
      return  0;
    }
  };

  struct mode
  {
    typedef mode_option<false>   slave;
    typedef mode_option<true>    master;
  };

  struct frame_size
  {
    typedef frame_size_option<3>   _4;
    typedef frame_size_option<4>   _5;
    typedef frame_size_option<5>   _6;
    typedef frame_size_option<6>   _7;
    typedef frame_size_option<7>   _8;
    typedef frame_size_option<8>   _9;
    typedef frame_size_option<9>   _10;
    typedef frame_size_option<10>  _11;
    typedef frame_size_option<11>  _12;
    typedef frame_size_option<12>  _13;
    typedef frame_size_option<13>  _14;
    typedef frame_size_option<14>  _15;
    typedef frame_size_option<15>  _16;
  };

  struct phase
  {
    typedef phase_option<false>    tx_on_rising_edge;
    typedef phase_option<true>     tx_on_falling_edge;
  };

  static constexpr void start()
  {
    specific::start(*spi_addr());
  }

  static constexpr void stop()
  {
    specific::stop(*spi_addr());
  }

  template<typename Options>
  static constexpr void 
  configure(Options p_opts)
  {
    /*
    spi_addr()->MCR     = SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK;
    spi_addr()->CTAR[0] = SPI_CTAR_PASC(2)    | 
                                  SPI_CTAR_ASC(1)     | 
                                  SPI_CTAR_PCSSCK(2)  | 
                                  SPI_CTAR_CSSCK(1)   | 
                                  SPI_CTAR_PBR(2)     | 
                                  SPI_CTAR_BR(4);
    p_opts.template accept<uint32_t>(spi<Specifier>(), null_op());*/
    spi_addr()->MCR |= SPI_MCR_MSTR_MASK | SPI_MCR_HALT_MASK;

    // static void init_fifo(void)
    spi_addr()->MCR &= ~SPI_MCR_MDIS_MASK;

    spi_addr()->MCR |= SPI_MCR_DIS_RXF_MASK |
                  SPI_MCR_DIS_TXF_MASK |
                  SPI_MCR_CLR_RXF_MASK |
                  SPI_MCR_CLR_TXF_MASK;

    // static void init_inactive_cs(void)
    spi_addr()->MCR |= SPI_MCR_PCSIS(1<<0) | SPI_MCR_PCSIS(1<<1);

    // static void init_frame_size(void)
    spi_addr()->CTAR[0] &= ~SPI_CTAR_FMSZ_MASK;
    spi_addr()->CTAR[0] |= SPI_CTAR_FMSZ(7) | 
                      // ((phase) ? SPI_CTAR_CPHA_MASK : 0u) | 
                      SPI_CTAR_PASC(2) | 
                      SPI_CTAR_ASC(1) | 
                      SPI_CTAR_PCSSCK(2) | 
                      SPI_CTAR_CSSCK(1) | 
                      SPI_CTAR_PBR(2) | 
                      SPI_CTAR_BR(4);    
    
    spi_addr()->MCR &= ~SPI_MCR_HALT_MASK;
  }
  
  static constexpr uint32_t
  read()
  {
    return SPI_POPR_RXDATA(spi_addr()->POPR);
  }
  
  static constexpr uint32_t 
  sync_xfer(uint32_t p_data, 
            uint32_t p_channel, 
            bool     p_end)
  {
    async_xfer(p_data, p_channel, p_end);

    wait_xfer();
    
    return  read();
  }

  static constexpr void 
  async_xfer(uint32_t p_data, 
             uint32_t p_channel, 
             bool     p_end)
  {
    uint32_t    ch_mask = mask(p_channel);
    if(p_end)
      spi_addr()->PUSHR = SPI_PUSHR_EOQ_MASK  |
                     SPI_PUSHR_PCS(ch_mask) |
                     SPI_PUSHR_TXDATA(p_data);
    else
      spi_addr()->PUSHR = SPI_PUSHR_CONT_MASK |
                     SPI_PUSHR_PCS(ch_mask) |
                     SPI_PUSHR_TXDATA(p_data);
  }

  static constexpr void
  wait_xfer()
  {
    while((spi_addr()->SR & SPI_SR_TCF_MASK) == 0);
    spi_addr()->SR |= SPI_SR_TCF_MASK;
  }

  static constexpr bool 
  is_xfer_completed()
  {
    bool result = false;
    if ((spi_addr()->SR & SPI_SR_TCF_MASK) == SPI_SR_TCF_MASK)
    {
      result = true;
      spi_addr()->SR |= SPI_SR_TCF_MASK;
    }
    return result;
  }
  
  static constexpr bool 
  is_tx_buffer_not_full()
  {
    return (bit_test(spi_addr()->SR, SPI_SR_TFFF_MASK));
  }

};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj


// HALUJ_BASE_DEVICES_ARM_KINETIS_SPI_HPP
#endif

