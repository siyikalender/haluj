/// \file modbus-rtu.hpp
/// Modbus RTU protocol implementation
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
/// \date   2020

#ifndef HALUJ_BASE_PROTOCOL_MODBUS_MODBUS_RTU_HPP
#define HALUJ_BASE_PROTOCOL_MODBUS_MODBUS_RTU_HPP

#include <cstdint>

#include "timer.hpp"
#include "state_machine.hpp"
#include "event_strategy.hpp"
#include "ring_buffer.hpp"
#include "bounded/vector.hpp"
#include "bitops.hpp"
#include "crc_ccitt.hpp"

namespace haluj
{
  
namespace base
{

namespace protocol
{

namespace modbus
{

// based on MODBUS over serial line specification and implementation guide V1.02 Figure 14
template<typename     Timer,
         typename     SerialDevice,
         typename     TxSelectPin,
         std::size_t  BufferSize>  
struct transmission_controller
{
private: // Types

  enum class states : uint8_t
  {
    null,
    initial_state,
    idle,
    reception,
    control_and_wait,
    emission
  };

public: // Types  

  typedef Timer                                 timer;    
  typedef typename Timer::duration              duration;
  typedef SerialDevice                          serial_device;
  typedef TxSelectPin                           tx_select_pin;    
  typedef bounded::vector<uint8_t, BufferSize>  buffer;

  enum diagnostic_counters
  {
    cpt_1,
    cpt_2,
    cpt_3,
    cpt_4,
    cpt_5,
    cpt_6,
    cpt_7,
    cpt_8,
    number_of_counters
  };

private: // Constants
  
  static constexpr uint8_t            c_frame_ok_bit            = 0U;
  static constexpr uint8_t            c_demand_of_emission_bit  = 1U;
  static constexpr uint8_t            c_emission_timer_bit      = 2U;

public:  // Constants

  static constexpr std::size_t        c_buffer_size             = BufferSize;

public: // Constructors and Destructor

  transmission_controller()
  {}
  
  transmission_controller(const transmission_controller&) = delete;

  transmission_controller(transmission_controller&& )     = delete;      
  
public: // Operator overloads
  
  template<typename EmissionCompleted,
           typename ReceptionCompleted>
  void operator()(EmissionCompleted   ec, 
                  ReceptionCompleted  rc)
  {
    using namespace haluj::base;
    
    auto append_rx = [&](const uint8_t c)
    {
      if (!bounded::push_back(rx_buffer_, c))
      {
        counters[cpt_8]++;
      }
      else
      {
        auto sz = rx_buffer_.size();
        if (sz >= 3)
        {
          crc.process_byte(rx_buffer_[sz - 3]);
        }
      }
    };
    
    auto e_initial_state = [&]()
    {
      timer_.set(duration_3c5_);
      flags_ = 0;
      tx_select_pin::clear();
    };
    
    auto a_initial_state = [&]()
    {
      if (serial_device::is_rx_available())
      {
        (void) serial_device::read(); // dummy read
        timer_.set(duration_3c5_);
      }
    };

    auto e_emission =  [&]()
    {
      crc.reset();
      clear_demand_of_emission();
      tx_index_ = 0;
      tx_select_pin::set();
    };
    
    auto a_emission =  [&]()
    {
      if (serial_device::is_tx_ready())
      {
        auto sz = tx_buffer_.size();
        if (tx_index_ < sz && tx_index_ < tx_buffer_.capacity())
        {
          uint8_t c = tx_buffer_[tx_index_];
          serial_device::write(c);
          if (tx_index_ < (sz - 2))
          {
            crc.process_byte(c);
          }
          tx_index_++;
          if (tx_index_ == (sz - 2))
          {
            tx_buffer_[tx_index_] = crc.crc_lo;
          }
          else if (tx_index_ == (sz - 1))
          {
            tx_buffer_[tx_index_] = crc.crc_hi;
          }
        }
        else
        {
          if (!bit_test(flags_, c_emission_timer_bit))
          {
            flags_ = bit_set(flags_, c_emission_timer_bit);
            timer_.set(duration_3c5_);
          }
        }
      }
      else
      {
        // ???
      }
    };

    auto l_emission =  [&]()
    {
      flags_ = bit_clear(flags_, c_emission_timer_bit);
      tx_select_pin::clear();
    };
    
    auto e_reception = [&]()
    {
      crc.reset();
      rx_buffer_.clear();
      bounded::push_back(rx_buffer_, uint8_t(serial_device::read())); // Beware cast
      timer_.set(duration_1c5_);
      clear_frame_ok();      
    };
    
    auto a_reception = [&]()
    {
      if (serial_device::is_error())
      {
        serial_device::clear_errors();
        counters[cpt_2]++;
      }
      else if (serial_device::is_rx_available())
      {
        append_rx(uint8_t(serial_device::read()));
        timer_.set(duration_1c5_);
      }
    };
    
    auto e_control_and_wait = [&]()
    {
      auto sz = rx_buffer_.size();
      
      if (sz >= 2)
      {
        auto crc_l  = rx_buffer_[sz - 2];
        auto crc_h  = rx_buffer_[sz - 1];

        auto rcv_crc = 
          crc_ccitt::value_type(crc_l) | 
          ( crc_ccitt::value_type(crc_h) << 8 );
        
        if (crc() == rcv_crc)
        {
          set_frame_ok();
          counters[cpt_1]++;
        }
        else
        {
          counters[cpt_2]++;
        }
      }
      else
      {
        counters[cpt_2]++;
      }
      
      timer_.set(duration_3c5_ - duration_1c5_);
    };
    
    auto a_control_and_wait = [&]()
    {
      if (serial_device::is_rx_available())
      {
        clear_frame_ok();
      }
    };
    
    // Guards
    
    switch(current_)
    {
    default:
      current_ = states::initial_state;
      e_initial_state();
      break;
    case states::initial_state:
      a_initial_state();
      if ( timer_(1, one_shot()) ) 
      { 
        current_ = states::idle; 
      }
      break;
    case states::idle:
      if (test_demand_of_emission())
      {
        current_ = states::emission;
        e_emission();
      }
      else if (serial_device::is_error())
      {
        serial_device::clear_errors();
        counters[cpt_2]++;
      }
      else if ( serial_device::is_rx_available() )
      {
        current_ = states::reception;
        e_reception();
      }
      break;
    case states::reception:
      a_reception();
      if ( timer_(1, one_shot()) ) 
      { 
        current_ = states::control_and_wait; 
        e_control_and_wait();
      }
      break;
    case states::control_and_wait:
      a_control_and_wait();
      if ( timer_(1, one_shot()) ) 
      { 
        rc(test_frame_ok());
        current_ = states::idle; 
      }
      break;
    case states::emission:
      a_emission();
      if ( timer_(1, one_shot()) ) 
      { 
        l_emission();
        ec();
        current_ = states::idle; 
      }
      break;
    }
  }

public: // Methods

  states current() const
  {
    return current_;
  }

  bool send(const uint8_t      p_slave_address,
            const uint8_t      p_function,
            const uint8_t*     p_data, 
            const std::size_t  p_size)
  {
    using namespace haluj::base;
          
    bool result = false;
    
    if (p_size <= (tx_buffer_.capacity() - 4U))
    {
      tx_buffer_.clear();
      
      bounded::push_back(tx_buffer_, p_slave_address);
      bounded::push_back(tx_buffer_, p_function);
      
      for (std::size_t i = 0; i < p_size; i++)
      {
        bounded::push_back(tx_buffer_, *(p_data + i));
      }
      
      bounded::push_back(tx_buffer_, uint8_t(0));
      bounded::push_back(tx_buffer_, uint8_t(0));

      set_demand_of_emission();
      
      result = true;
    }
    
    return result;
  }    
  
  void set_delays(const int32_t p_duration_0c5)
  {
    duration_1c5_ = p_duration_0c5 * 3;
    duration_3c5_ = p_duration_0c5 * 7;
  } 

  bool is_busy() const
  {
    return current_ != states::idle;
  }
  
  bool is_frame_ok() const
  {
    return test_frame_ok();
  }
  
  void clear_counters()
  {
    std::fill_n(&counters[0], number_of_counters, 0);
  }
  
  void reset()
  {
    current_ = states::null;
  }
  
private: // Methods  
  
  void set_demand_of_emission()
  {
    flags_ = bit_set(flags_, c_demand_of_emission_bit);
  }

  void clear_demand_of_emission()
  {
    flags_ = bit_clear(flags_, c_demand_of_emission_bit);
  }
  
  bool test_demand_of_emission() const
  {
    return bit_test(flags_, c_demand_of_emission_bit);
  }
  
  void set_frame_ok()
  {
    flags_ = bit_set(flags_, c_frame_ok_bit);
  }

  void clear_frame_ok()
  {
    flags_ = bit_clear(flags_, c_frame_ok_bit);
  }
  
  bool test_frame_ok() const
  {
    return bit_test(flags_, c_frame_ok_bit);
  }

public: // Members

  uint8_t                         flags_;
  states                          current_ = states::null;
  crc_ccitt                       crc;
  uint32_t                        tx_index_;
  buffer                          rx_buffer_;
  buffer                          tx_buffer_;
  timer                           timer_;
  duration                        duration_1c5_;
  duration                        duration_3c5_;
  uint16_t                        counters[number_of_counters];

};

template<typename     Timer,
         typename     SerialDevice,
         typename     TxSelectPin,
         std::size_t  BufferSize>
struct master
{
  // based on MODBUS over serial line specification and implementation guide V1.02 Section 2.4.1
  
private: // Types

  enum class states : uint8_t
  {
    null,
    idle,
    waiting_turnaround_delay,
    waiting_for_reply,
    processing
  };

public: // Types

  typedef Timer                       timer;
  typedef typename Timer::duration    duration;
  typedef SerialDevice                serial_device;
  typedef TxSelectPin                 tx_select_pin;
  typedef transmission_controller<Timer, SerialDevice, TxSelectPin, BufferSize>  core;

private: // Constants

  static constexpr uint8_t  c_broadcast_bit       = 0U;
  static constexpr uint8_t  c_unicast_bit         = 1U;
  static constexpr uint8_t  c_received_reply_bit  = 2U;
  static constexpr uint8_t  c_error_bit           = 3U;

public:  // Constants
  
  static constexpr int32_t c_default_wait_turnaround = 100000;  // in ticks
  static constexpr int32_t c_default_wait_reply      = 2000000; // in ticks

public: // Constructors and Destructor
  
  master()
  : current_(states::null),
    active_address_(0),
    flags_(0),
    duration_wait_reply_(c_default_wait_reply),
    duration_wait_turnaround_(c_default_wait_turnaround)
  {}
  
  master(const master&) = delete;

  master(master&& ) = delete;
  
  ~master() {}
  
public: // Operator overloads
  
  template<typename Process, typename Timeout>
  void operator()(Process  process, Timeout timeout)
  {
    using namespace haluj::base;
    
    auto &b           = core_.rx_buffer_;      
    
    core_(
      [&]() {
        if (active_address_ == 0)
        {
          set_broadcast();
          timer_.set(duration_wait_turnaround_);
        } 
        else
        {
          set_unicast();
          timer_.set(duration_wait_reply_);
        }
      },  // emission completed
      [&](bool frame_ok) {
        set_received_reply();
      }
    );      
    
    auto g_end_of_processing = [&]() -> bool
    {
      return process(!core_.is_frame_ok() || test_error(), b); // frame_ok
    };
    
    switch(current_)
    {
    default:
      current_ = states::idle;
      b.clear();
      break;
    case states::idle:
      clear_error();
      if (test_broadcast())
      {
        clear_broadcast();
        current_ = states::waiting_turnaround_delay;
      }
      else if (test_unicast())
      {
        clear_unicast();
        current_ = states::waiting_for_reply;
      }
      break;
    case states::waiting_turnaround_delay:
      if(timer_(1, one_shot()))
      {
        timeout();
        current_ = states::null;
      }
      break;
    case states::waiting_for_reply:
      if (timer_(1, one_shot()))
      {
        set_error();
        current_ = states::processing; 
      }
      else if (test_received_reply())
      {
        clear_received_reply();
        current_ = states::processing;
      }
      
      break;
    case states::processing:
      if (g_end_of_processing())
      {
        current_ = states::null;
      }
      break;
    }
  }

public: // Methods

  bool is_busy() const
  {
    return core_.is_busy() && current() != states::idle;
  }

  states current() const
  {
    return current_;
  }
  
  bool send(const uint8_t      p_slave_address,
            const uint8_t      p_function,
            const uint8_t*     p_data, 
            const std::size_t  p_size)
  {
    active_address_ = p_slave_address;
    return core_.send(p_slave_address, p_function, p_data, p_size);
  }    

  void clear_counters()
  {
    core_.clear_counters();
  }
  
  void reset()
  {
    current_                  = states::null;
    active_address_           = 0;
    flags_                    = 0;
    duration_wait_reply_      = c_default_wait_reply;
    duration_wait_turnaround_ = c_default_wait_turnaround;
    core_.reset();
  }

  void set_delays(const int32_t p_wait_reply,
                  const int32_t p_wait_turnaround,
                  const int32_t p_duration_0c5)
  {
    duration_wait_reply_      = p_wait_reply;
    duration_wait_turnaround_ = p_wait_turnaround;
    core_.set_delays(p_duration_0c5);
  }

private: // Methods

  void set_broadcast() 
  {
    flags_ = bit_set(flags_, c_broadcast_bit);
  }
  
  void clear_broadcast() 
  {
    flags_ = bit_clear(flags_, c_broadcast_bit);
  }

  bool test_broadcast() const
  {
    return bit_test(flags_, c_broadcast_bit);
  }
  
  void set_unicast() 
  {
    flags_ = bit_set(flags_, c_unicast_bit);
  }
  
  void clear_unicast() 
  {
    flags_ = bit_clear(flags_, c_unicast_bit);
  }
  
  bool test_unicast() const
  {
    return bit_test(flags_, c_unicast_bit);
  }

  void set_received_reply() 
  {
    flags_ = bit_set(flags_, c_received_reply_bit);
  }
  
  void clear_received_reply() 
  {
    flags_ = bit_clear(flags_, c_received_reply_bit);
  }
  
  bool test_received_reply() const
  {
    return bit_test(flags_, c_received_reply_bit);
  }

  void set_error() 
  {
    flags_ = bit_set(flags_, c_error_bit);
  }
  
  void clear_error() 
  {
    flags_ = bit_clear(flags_, c_error_bit);
  }
  
  bool test_error() const
  {
    return bit_test(flags_, c_error_bit);
  }

private: // Members

  states        current_;
  uint8_t       active_address_;
  uint8_t       flags_;
  duration      duration_wait_reply_;
  duration      duration_wait_turnaround_;
  core          core_;
  timer         timer_;

};

// based on MODBUS over serial line specification and implementation guide V1.02 Section 2.4.1

template<typename     Timer,
         typename     SerialDevice,
         typename     TxSelectPin,
         std::size_t  BufferSize>
struct slave
{
private: // Types

  enum class states : uint8_t
  {
    null,
    idle,
    checking_request,
    processing_required_action,
    formatting_reply
  };

public: // Types 

  typedef transmission_controller<Timer, SerialDevice, TxSelectPin, BufferSize>  core;
  typedef typename core::duration     duration;

private: // Contants

  static constexpr uint8_t c_reception_of_request_bit        = 0U;
  static constexpr uint8_t c_reply_sent_bit                  = 1U;
  static constexpr uint8_t c_is_broadcast_bit                = 2U;

public: // Constructors and Destructor

  slave()
  : current_(states::null),
    address_(1),
    flags_(0)
  {}
  
  slave(const slave&) = delete;

  slave(slave&& ) = delete;
  
  ~slave() {}

public: // Operator overloads

  template<typename Check, typename Process>
  void operator()(Check p_check, Process p_process)
  {
    using namespace haluj::base;
    
    auto &b           = core_.rx_buffer_;
    
    core_(
      [&]() {
        set_reply_sent();
      },  // emission completed
      [&](bool frame_ok) {
        set_reception_of_request();
        flags_ = bit_assign(flags_, c_is_broadcast_bit, (b.size() > 0) && (b[0] == 0));
      }   // reception completed
    );

    auto g_frame_nok_not_my_msg = [&]() -> bool
    {
      bool p  = (b.size() == 0) || !((b[0] == address_) || test_is_broadcast());
      return !core_.is_frame_ok() || p;
    };

    switch(current_)
    {
    default:
      current_ = states::idle;
      b.clear();
      break;
    case states::idle:
      if (test_reception_of_request())
      {
        clear_reception_of_request();
        current_ = states::checking_request;
      }
      break;
    case states::checking_request:
      {
        if (g_frame_nok_not_my_msg())
        {
          current_ = states::null;
        }
        else 
        {
          if (!p_check(b)) 
          {
            current_ = (test_is_broadcast()) ? states::null : states::formatting_reply;
          }
          else
          {
            current_ = states::processing_required_action;
          }
        }
      }
      break;
    case states::processing_required_action:
      if (p_process(test_is_broadcast(), b)) 
      {
        current_ = (test_is_broadcast()) ? states::null : states::formatting_reply;
      }
      break;
    case states::formatting_reply:
      if (test_reply_sent())
      {
        clear_reply_sent();
        current_ = states::null;
      }      
      break;
    }
  }

public: // Methods  

  bool send(const uint8_t      p_function,
            const uint8_t*     p_data, 
            const std::size_t  p_size)
  {
    return core_.send(address_, p_function, p_data, p_size);
  }
  
  void set_address(const uint8_t p_address)
  {
    address_ = p_address;
  }
  
  void clear_counters()
  {
    core_.clear_counters();
  }
  
  bool is_busy() const
  {
    return (current_ != states::idle);
  }

  states current() const
  {
    return current_;
  }
  
  void reset()
  {
    current_ = states::null;
    address_ = 1;
    flags_   = 0;
    core_.reset();
  }

  void set_delays(const int32_t p_duration_0c5)
  {
    core_.set_delays(p_duration_0c5);
  }

private: // Methods

  void set_reception_of_request() 
  {
    flags_ = bit_set(flags_, c_reception_of_request_bit);
  }
  
  void clear_reception_of_request() 
  {
    flags_ = bit_clear(flags_, c_reception_of_request_bit);
  }

  bool test_reception_of_request() const
  {
    return bit_test(flags_, c_reception_of_request_bit);
  }
  
  void set_reply_sent() 
  {
    flags_ = bit_set(flags_, c_reply_sent_bit);
  }
  
  void clear_reply_sent() 
  {
    flags_ = bit_clear(flags_, c_reply_sent_bit);
  }
  
  bool test_reply_sent() const
  {
    return bit_test(flags_, c_reply_sent_bit);
  }
  
  void set_is_broadcast() 
  {
    flags_ = bit_set(flags_, c_is_broadcast_bit);
  }
  
  void clear_is_broadcast() 
  {
    flags_ = bit_clear(flags_, c_is_broadcast_bit);
  }
  
  bool test_is_broadcast() const
  {
    return bit_test(flags_, c_is_broadcast_bit);
  }

private: // Members

  states      current_;
  uint8_t     address_;
  uint8_t     flags_;
  core        core_;

};
  
} // namespace modbus

} // namespace protocol

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_PROTOCOL_MODBUS_MODBUS_RTU_HPP
