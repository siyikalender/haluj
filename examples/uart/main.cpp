/// \file uart.cpp
/// Example code for uart usage. This example demonstrates an 
/// implementation of a echo terminal
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

using namespace haluj::base;
using namespace haluj::base::devices::arm::kinetis;
using namespace haluj::base::devices::arm::kinetis::specific;

int main()
{
  // open function enables the clock gating
  open<uart_0>(); // depending on the device other uarts may be available
  open<port_d>(); // pin 6 and 7 of port d is used for uart
  
  // configure port zero to uart function. 
  // Configuration is MK60D100 100 pin devices specific
  // Please modify for the intended device
  port_d::configure(6, options(port_d::mux::_3,     // uart 0 rx
                               port_d::pull::up));

  port_d::configure(7, options(port_d::mux::_3));   // uart 0 tx

  // configure function, disables transmit and receive, 
  // then applies options, 
  // finally enables transmit and receive
  uart_0::configure( 
    options(uart_0::baud_rate<9600>(), 
            uart_0::bits::_8, 
            uart_0::parity::none,  
            uart_0::stop_bits::one)); // all  default: 9600, 8 bits, No partiy, 1 stop bits;

  uint8_t d = 0;

  while (char(d) != 'q')
  {
    while(!uart_0::is_rx_available());  // wait for byte available
    d = uart_0::read();                 // read byte
    while(!uart_0::is_tx_ready());      // wait for transmitter is ready
    uart_0::write(d);                   // send byte
  }
  // close function disables the clock gating
  close<uart_0>();
  close<port_d>();
  
  return 0;
}
