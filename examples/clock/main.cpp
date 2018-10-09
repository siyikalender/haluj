/// \file examples/clock/main.cpp
/// Example code for clock usage
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
#include "timer.hpp"

using namespace std::chrono;
using namespace haluj::base;
using namespace haluj::base::devices::arm;
using namespace haluj::base::devices::arm::kinetis;
using namespace haluj::base::devices::arm::kinetis::specific;

constexpr uint32_t c_test_pin = 15;

int main()
{
  open<port_a>();
  
  port_a::configure(c_test_pin, options(port_a::mux::_1));
  
  port_a::set_direction(mask(c_test_pin));
  
  periodic_timer<precision_clock::time_point, milliseconds>    
    tmr(milliseconds(1000)); // software timers are clock agnostic

  auto now = precision_clock::initialize();
  
  tmr.start(now);
  
  for (;;)
  {
    now = precision_clock::now(); // clock interface is based on chrono
    
    tmr(
      now, 
      [&]() {
        port_a::toggle(mask(c_test_pin));
      }
    );
  }
  
  return 0;
}
