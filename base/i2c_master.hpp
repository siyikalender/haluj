/// \file i2c_master.hpp
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
/// \author Selcuk Iyikalender
/// \date   2018

#ifndef I2C_MASTER_HPP
#define I2C_MASTER_HPP

#include <cstdint>
#include <array>

#include "i2c.hpp"
#include "ring_buffer.hpp"
#include "set_and_wait.hpp"
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
         unsigned     StartTimeOut,
         unsigned     XferTimeOut,
         unsigned     StopTimeOut>
struct i2c_master
{
  typedef TimePoint   time_point;
  typedef Duration    duration;
  
  static constexpr unsigned c_tx_buffer_size  = TxBufferSize;
  static constexpr unsigned c_rx_buffer_size  = RxBufferSize;
  static constexpr unsigned c_start_time_out  = StartTimeOut;
  static constexpr unsigned c_xfer_time_out   = XferTimeOut;
  static constexpr unsigned c_stop_time_out   = StopTimeOut;
  
  enum class states
  {
    idle            = 0,
    wait_busy       = 1,
    start           = 2,
    tx_address      = 3,
    tx_data         = 4,
    repeated_start  = 5,
    rx_address      = 6,
    rx_initiate     = 7,
    rx_data         = 8,
    finalize        = 9
  };
  
  i2c_master()
  : m_state(states::idle),
    m_read_count(0),
    m_timer()
  {
    static_assert(TxBufferSize > 0, "Tx Buffer Size must be greater than 0");
    static_assert(RxBufferSize > 0, "Rx Buffer Size must be greater than 0");
  }
  
  void operator ()(const time_point p_now)
  {
    m_saw(
      [&]()   // SET function
      {
        duration delay = duration(0);
        
        switch(m_state)
        {
        case states::start: // Generate Start
          T::enable_tx();
          T::start(); // Generate a start signal which sets SDA line to low.
          delay = duration(c_start_time_out);
          break;
        case states::tx_address: // Send Address
          T::write(m_slave_address);
          delay = duration(c_xfer_time_out);
          break;
        case states::tx_data: // Send Data
          {
            if (!m_tx_buffer.empty())
            {
              T::write(m_tx_buffer.front());
              m_tx_buffer.pop();
              delay = duration(c_xfer_time_out);
            }
          }
          break;
        case states::repeated_start: // repeated start
          T::repeated_start();
          delay = duration(c_start_time_out);
          m_rx_buffer.clear();
          break;
        case states::rx_address:
          T::write(m_slave_address | 0x01); // read mode
          delay = duration(c_xfer_time_out);
          break;
        case states::rx_initiate: 
          T::disable_tx();
          if (m_read_count == 1) T::disable_ack();    // For single byte read
          T::read();                                  // dummy read for start clocking
          delay = duration(c_xfer_time_out);
          break;
        case states::rx_data: 
          if (m_read_count == 2) T::disable_ack(); // second to last byte
          if (m_read_count == 1) T::stop();
          if (!m_rx_buffer.full())
            m_rx_buffer.push(T::read());
          // else
            // raise error
          delay = duration(c_xfer_time_out);
          break;
        case states::finalize:
          T::idle();
          delay = duration(c_stop_time_out);
          break;
        case states::wait_busy:
          delay = duration(c_xfer_time_out);
        case states::idle:
        default:
          break;
        }
        m_timer.set(p_now + delay);
        m_timer.start();
      },
      [&]() -> bool // Test Function
      {
        bool result = false;
        bool timeout = m_timer(p_now);
        switch(m_state)
        {
        case states::wait_busy:
          result = timeout || !T::test(T::busy);
          break;
        case states::start:
        case states::repeated_start:
          result = timeout || T::test(T::busy);
          break;
        case states::tx_address:
        case states::tx_data:
        case states::rx_address:
        case states::rx_initiate:
          result = timeout && T::test(T::iicif);
          break;
        case states::rx_data:
          result = timeout && ((m_read_count == 1) || T::test(T::iicif));
          break;
        case states::idle:
        case states::finalize:
        default:
          result = true;
          break;
        }
        return result;
      },
      [&]() // Test Success Function
      {
        switch(m_state)
        {
        case states::wait_busy:
          m_state = T::test(T::busy) ? states::finalize : states::start;
          break;
        case states::start:
          m_state = !T::test(T::busy) ? states::finalize : states::tx_address;
          break;
        case states::tx_address:
          m_state = (!T::test(T::iicif) || 
                      T::test(T::rxak)  || 
                      T::test(T::arbl)) ? states::finalize : states::tx_data;
          T::clear_iicif();
          T::clear_arbl();
          break;
        case states::tx_data:
          if (!T::test(T::iicif) || T::test(T::rxak))
          {
            m_state = (m_read_count == 0) ? states::finalize : states::repeated_start;
          }
          else
          {
            if (m_tx_buffer.empty())
            {
              m_state = (m_read_count == 0) ? states::finalize : states::repeated_start;
            }
            else
            {
              m_state  =  states::tx_data;
            }
          }
          T::clear_iicif();
          break;
        case states::repeated_start:
          m_state = !T::test(T::busy) ? states::finalize : states::rx_address;
          break;
        case states::rx_address:
          m_state = (!T::test(T::iicif) || 
                      T::test(T::rxak)  || 
                      T::test(T::arbl)) ? states::finalize : states::rx_initiate;
          T::clear_iicif();
          T::clear_arbl();
          break;
        case states::rx_initiate:
          m_state = !T::test(T::iicif) ? states::finalize : states::rx_data;
          T::clear_iicif();
          break;
        case states::rx_data:
          m_read_count--;
          m_state = (!T::test(T::iicif) || 
                    (m_read_count == 0)) ? states::finalize : states::rx_data;
          T::clear_iicif();                            
          break;
        default:
          m_state = states::idle;
          break;
        }
      }
    );
  }
  
  bool read(const uint8_t     /* p_slave_address */,
            uint8_t*          p_data, 
            const std::size_t p_read_size)
  {
    bool  result = false;
    
    if (!is_busy() && !m_rx_buffer.empty())
    {
      // Completed transfer
      if (p_read_size <= m_rx_buffer.size()) 
      {
        for (std::size_t i = 0; i < p_read_size; i++)
        {
          p_data[i] = m_rx_buffer.front();
          m_rx_buffer.pop();
        }
        result = true;
      }
      m_rx_buffer.clear(); // Clear remaining data
    }
    
    return result;
  }
  
  bool write(const uint8_t      p_slave_address,
             const uint8_t*     p_data, 
             const std::size_t  p_write_size,
             const std::size_t  p_read_size = 0)
  {
    bool result = false;
    
    if (p_write_size <= m_tx_buffer.size() && !is_busy())
    {
      m_tx_buffer.clear();
      
      for (std::size_t i = 0; i < p_write_size; i++)
      {
        m_tx_buffer.push(*(p_data + i));
      }

      m_slave_address = p_slave_address;
      m_read_count    = p_read_size;
      m_state         = states::wait_busy;
      
      result = true;
    }
    
    return result;
  }
  
  bool is_busy() const
  {
    return (m_state != states::idle);
  }
  
  set_and_wait                                        m_saw;  
  states                                              m_state;
  unsigned                                            m_read_count;
  ring_buffer<std::array<uint8_t, c_tx_buffer_size>>  m_tx_buffer;
  ring_buffer<std::array<uint8_t, c_rx_buffer_size>>  m_rx_buffer;
  one_shot_timer<time_point>                          m_timer;
  uint8_t                                             m_slave_address;
};

} // namespace base

} // namespace haluj

// I2C_MASTER_HPP
#endif
