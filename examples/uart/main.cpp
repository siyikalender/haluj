/// \file uart.cpp
/// Example code for uart usage
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
#include "specific.hpp"

using namespace haluj::base;
using namespace haluj::base::devices::arm::kinetis;
using namespace haluj::base::devices::arm::kinetis::specific;

int main()
{
  // open function enables the clock gating of the peripheral
  open(uart_0);
  open(uart_1);
  open(uart_2);

  configure(uart_0); // all  default: 9600, 8 bits, No partiy, 1 stop bits;
  // bits::_9 and stop_bits::two may not by supported in every device
  configure(uart_1, 115200, bits::_8, parity::odd,  stop_bits::one);
  configure(uart_2, 250000, bits::_9, parity::even, stop_bits::one);

  // close function disables the clock gating
  close(uart_0);
  close(uart_1);
  close(uart_2);

  return 0;
}
