/// \file i2c_slave.hpp
/// Controller for Master I2C Operation
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

#ifndef I2C_SLAVE_HPP
#define I2C_SLAVE_HPP

#include <cstdint>
#include <array>

#include "i2c.hpp"
#include "ring_buffer.hpp"
#include "optional.hpp"
#include "event_strategy.hpp"
#include "timer.hpp"

namespace haluj
{
  
namespace base
{

template<typename     T,
         std::size_t  TxBufferSize,
         std::size_t  RxBufferSize,
         typename     TimePoint,
         typename     Duration,
         unsigned     RxTimeout>
struct i2c_slave
{
  typedef TimePoint       time_point;
  typedef Duration        duration;
  
  static constexpr unsigned c_tx_buffer_size  = TxBufferSize;
  static constexpr unsigned c_rx_buffer_size  = RxBufferSize;
  static constexpr unsigned c_rx_timeout      = RxTimeout;
  
  enum class events
  {
    address_received
  };

  i2c_slave() 
  : m_timer(),
    m_read_mode(false)
  {
    static_assert(TxBufferSize > 0, "Tx Buffer Size must be greater than 0");
    static_assert(RxBufferSize > 0, "Rx Buffer Size must be greater than 0");    
  }
  
  void handler()
  {
    auto addressed_as_slave = [&]()
    {
      m_event.raise(events::address_received);
      
      m_read_mode = T::test(T::srw);
      
      if (m_read_mode)
      {
        // Master reads from device
        T::enable_tx();
        if (!m_tx_buffer.empty())
        {
          T::write(m_tx_buffer.front());
          m_tx_buffer.pop();
        }
        else
          T::write(0);
      }
      else
      {
        T::disable_tx();
        // Master writes to device
        // T::disable_ack();
        (void) T::read(); // dummy read
      }
    };
    
    // could be in interrupt handler
    if (T::test(T::iicif))
    {
      T::clear_iicif();
      
      if (T::test(T::arbl))
      {
        // Arbitration is lost
        T::clear_arbl();
        
        if (T::test(T::iaas))
        {
          addressed_as_slave();
        }
      }
      else
      {
        // Arbitration not lost
        if (T::test(T::iaas))
        {
          addressed_as_slave();
        }
        else
        {
          if (T::is_tx_enabled())
          {
            if (T::test(T::rxak))
            {
              // last byte received. release bus to master control
              T::disable_tx();
              if (!m_rx_buffer.full())
                m_rx_buffer.push(T::read());
              // else
                // raise error
            }
            else
            {
              if (!m_tx_buffer.empty())
              {
                T::write(m_tx_buffer.front());
                m_tx_buffer.pop();
              }
              else
                T::write(0);
            }
          }
          else
          {
            // receiving second to last byte
            auto remaining = m_rx_buffer.remaining();

            if (remaining == 2)
            {
              T::disable_ack(); // NOTE ! Additionally Disable ACK may be required after read
            }
            
            if (!m_rx_buffer.full())
              m_rx_buffer.push(T::read());
            // else
              // raise error
            
            if (remaining == 1)
            {
              // Enable ACK again for next transfer
              // ACK change is effective on next byte transfer
              // due to the Kinetis implementation (see FACK bit)
              // T::enable_ack(); 
            }            
          }
        }
      }
    }
  }
  
  template<typename AddressReceivedFunction,
           typename TimeoutFunction>
  void operator ()(const time_point&       p_now, 
            AddressReceivedFunction arf,
            TimeoutFunction         tof)
  {
    // Poll the handler. Altenatively handler may be used in interrupt handler
    handler();
    
    if (m_event.test(events::address_received)) 
    {
      m_event.clear();
      m_rx_buffer.clear();
      m_timer.set(p_now + duration(c_rx_timeout));
      arf(m_read_mode);
    }
    
    m_timer(
      p_now, 
      [&]()
      {
        m_event.clear();
        tof();
        // slave_device.enable_ack(); NOTE ! May be required (When stuck in NACK)
        // slave_device.read();       NOTE ! May be required (When stuck in NACK)
      }    
    );    
    
  }

  bool read(uint8_t*            p_data, 
            const std::size_t   p_size)
  {
    bool  result = false;
    
    if ((p_size <= m_rx_buffer.size()) && !is_busy())
    {
      for (std::size_t i = 0; i < p_size; i++)
      {
        p_data[i] = m_rx_buffer.front();
        m_rx_buffer.pop();
      }

      result = true;
    }
    
    return result;
  }
  
  bool write(const uint8_t*     p_data, 
             const std::size_t  p_size)
  {
    bool result = false;
    // Buffer must be clean up for correct operation.
    m_tx_buffer.clear(); 
    
    if (p_size <= m_tx_buffer.size() && !is_busy())
    {
      
      for (std::size_t i = 0; i < p_size; i++)
      {
        m_tx_buffer.push(p_data[i]);
      }

      result = true;
    }
    
    return result;
  }

  bool is_busy() const
  {
    return false; // TBD
  }

  one_shot_timer<time_point>                          m_timer;
  blackboard_strategy<events>                         m_event;
  bool                                                m_read_mode;
  ring_buffer<std::array<uint8_t, c_tx_buffer_size>>  m_tx_buffer;
  ring_buffer<std::array<uint8_t, c_rx_buffer_size>>  m_rx_buffer;
};

} // namespace base

} // namespace haluj

#endif // I2C_SLAVE_HPP
