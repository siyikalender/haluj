/// \file gpio.cpp
/// Example code for port control demonstration
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
  // open function enables the clock gating of the peripheral
  open(port_a);
  // open(port_b);
  // open(port_c);
  // open(port_d);
  // open(port_e);
  
  // configure port zero to appropriate function
  configure(port_a, 2, options(mux::_1, pull::down, slew_rate::slow));

  // create mask for bits 1, 2, 3 that makes 0xE
  uint32_t m = mask(1U, 2U, 3U);
  // set direction by setting the mask
  set_direction(port_a, m);
  set(port_a,    m);
  clear(port_a,  m);
  toggle(port_a, m);
  write(port_a, m);
  uint32_t r = read(port_a);

  // close function disables the clock gating
  close(port_a);
  // close(port_b);
  // close(port_c);
  // close(port_d);
  // close(port_e);

  return 0;
}
