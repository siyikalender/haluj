/// \file examples/i2c/main.cpp
/// Example code for I2C usage
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

#include "bitops.hpp"
#include "peripherals.hpp"
#include "clock.hpp"
#include "i2c_master.hpp"
#include "i2c_slave.hpp"
#include "systick.hpp"

using namespace haluj::base;
using namespace haluj::base::devices::arm;
using namespace haluj::base::devices::arm::kinetis;
using namespace haluj::base::devices::arm::kinetis::specific;

using systick_periodic_timer    = timer<systick, periodic>;

int main()
{
  // open function enables the clock gating
  open<i2c_0>(); // depending on the device other uarts may be available
  open<i2c_1>(); // depending on the device other uarts may be available

  i2c_0::configure(
    options(
      i2c_0::address<0x53>(),
      i2c_0::baud_rate<400000>()
    )
  );

  i2c_1::configure(
    options(
      i2c_1::address<0x54>(),
      i2c_1::baud_rate<400000>()
    )
  );
  
  auto now = precision_clock::initialize();
  
  systick_periodic_timer      tmr_0;
  
  i2c_master<i2c_0, 8, 8,  int, 2, 6, 2> master;
  i2c_slave<i2c_1,  8, 8,  int, 80> slave;

  // These variables are related to slave example use case
  unsigned        master_message_state  = 0U;
  unsigned        r_index               = 0U;
  unsigned        current_index         = 0U;
  uint8_t         rx_buf[8];

  tmr_0.set(system_core_clock() / 40000); // 40kHz -> 25uS

  for (;;)
  {
    tmr_0(
      0, // omit this parameter for systick or other hardware timers
      [&]() 
      {
        ///////////// Master
        master(1);
        
        ///////////// Slave 
        slave(
          1,
          [&](bool p_read_mode)  // Address received handler
          {
            master_message_state = (p_read_mode) ? 0 : 1;        
          },
          [&]() // Timeout handler, to clear things up
          {
            master_message_state  = 0;
            r_index             = 0;        
          } 
        );
          
        uint8_t   d;
        // Process received message. This is an example use case. 
        while(slave.read(&d, 1))
        {
          switch(master_message_state)
          {
          default: // case 0
            break;
          case 1: // Access to Command Register
            {
              // Data holds the register
              master_message_state  = 2;
              current_index         = 0;
            }
            break;
          case 2: // Receive  INCOMING DATA
            {
              if (++current_index >= sizeof(rx_buf))
              {
                current_index         = 0;
                // Process data on transition
                master_message_state  = 0;
              }
            }
            break;
          }
        }
      }
    );
  }
  
  // stop(i2c_0);
  // close function disables the clock gating
  // close<i2c_0>();
  
  return 0;
}
