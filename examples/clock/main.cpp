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
#include "pin.hpp"
#include "peripherals.hpp"
#include "timer.hpp"
#include "systick.hpp"
#include "timer_implementations/software.hpp"

using namespace haluj::base;
using namespace haluj::base::devices::arm;
using namespace haluj::base::devices::arm::kinetis;
using namespace haluj::base::devices::arm::kinetis::specific;
using namespace haluj::base::timer_implementations;

using blue    = pin<port_b, 21>;
using red     = pin<port_b, 22>;

// counter based software timer special type definitions
// software timer implementation can be forward (up counting) or backward (down counting) in direction.
// the duration type can be based on int, double, chrono clock duration. Use of signed types is recommended.
using fwd_int_timer    = timer<software::implementation<software::forward<int>>>;
// using bwd_int_periodic_timer    = timer<software::implementation<software::backward<int>>, periodic>;
// using fwd_chrono_one_shot_timer = timer<software::implementation<software::forward<std::chrono::system_clock::duration>>,  one_shot>;

// ARM architecture systick based periodic timer definition
using systick_timer    = static_timer<systick>;

int main()
{
  open<port_b>();
  
  red::configure(options<port_b::mux::_1>());
  red::configure(options<port_b::mux::_1>());
  
  port_b::set_direction(mask(red::index, blue::index));
  
  red::set();
  blue::set();
  
  // Instantiate software timers
  fwd_int_timer    tmr_1;
  
  // load and start the timers
  // systick is a hardware timer. So the interface is based on static methods.
  systick_timer::set(system_core_clock() / 10); // sets ans starts timer
  // set software timer count value
  tmr_1.set(5);

  for (;;)
  {
    // process timers
    systick_timer::poll(
      0, // this value is omitted for systick or other hardware timers. Just give a zero value
      // lambda for tmr_0 timeout
      [&]() {
        red::toggle();
        tmr_1(
          1, // delta 1, increase (for forward implementation) count value by 1
          // lambda from tmr_1 timeout
          [&] {
            blue::toggle();
            return false; // return false for periodic operation 
          }
        );
        return false; // return false for periodic operation
      }
    );
  }

  return 0;
}
