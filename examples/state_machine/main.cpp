/// \file examples/clock/main.cpp
/// Example code for state_machine usage
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
#include "timer.hpp"
#include "pin.hpp"
#include "state_machine.hpp"
#include "timer.hpp"
#include "systick.hpp"
#include "timer_implementations/software.hpp"


using namespace haluj::base;
using namespace haluj::base::devices::arm;
using namespace haluj::base::devices::arm::kinetis;
using namespace haluj::base::devices::arm::kinetis::specific;
using namespace haluj::base::timer_implementations;

/// This example demonstrates a simple indicator light implementation
/// based on state_machine.hpp. 

/// In the example it is assumed there are three indicators represented
/// as red, yellow and green, like in some traffic lights.
/// Initially the red indicator is turned on and then it cycles in order
/// yellow, green and red again. Transition conditions depends on timer
/// precision_clock is used to determine the duration of each state.

/// This state machine implemenation produces considerable efficient
/// object code, depending on the optimization and the complexity. 
/// As the complexity increases, unfortunately it is quite possible 
/// the produced object code size folds, comparing to other 
/// implementations based on switch

enum class states
{
  null,
  red,
  yellow,
  green
};

// If required, please change these definitions.
using red_light_pin     = pin<port_c, 0>;
using yellow_light_pin  = pin<port_c, 1>;
using green_light_pin   = pin<port_c, 2>;

using fwd_int_timer    = timer<software::implementation<software::forward<uint32_t>>>;
using systick_timer    = static_timer<systick>;

constexpr uint32_t    c_systick_hz = 1000; // 1000Hz -> 1ms period

int main()
{
  open<port_c>();

  // Set GPIO
  red_light_pin::configure(options<port_c::mux::_1>());
  yellow_light_pin::configure(options<port_c::mux::_1>());
  green_light_pin::configure(options<port_c::mux::_1>());

  port_c::set_direction(
    mask(
      red_light_pin::index, 
      yellow_light_pin::index, 
      green_light_pin::index
    )
  );

  fwd_int_timer      transition_timer;
  
  systick_timer::set(system_core_clock() / c_systick_hz); 
  
  // lambda functions for guards.
  // Note that, g_always guard is predefined
  auto g_timeout = [&]() -> bool 
  {
    return transition_timer(1, one_shot());
  };
  
  // lambda functions for state actions
  auto e_red     = [&]()
  {
    // start the timer
    transition_timer.set(5000); // 5 secs
    // turn on the light
    red_light_pin::set();
  };

  auto l_red     = [&]()
  {
    // turn off the light
    red_light_pin::clear();
  };

  auto e_yellow  = [&]()
  {
    transition_timer.set(2000); // 2 secs
    yellow_light_pin::set();
  };

  auto l_yellow  = [&]()
  {
    yellow_light_pin::clear();
  };

  auto e_green   = [&]()
  {
    transition_timer.set(8000); // 8 secs
    green_light_pin::set();
  };

  auto l_green   = [&]()
  {
    green_light_pin::clear();
  };

  // the state machine
  
  auto m = 
    machine(
      graph(
        transition(g_always , states::null,     states::red),
        transition(g_timeout, states::red,      states::yellow /*, ta_red_to_yellow */ ), // it is possible to implement transition actions like it is demonstrated in the comment
        transition(g_timeout, states::yellow,   states::green),
        transition(g_timeout, states::green,    states::red)
      ),
      map( // map for state actions. at least one entry is required
                                       //  enter, do    , leave 
        entry(states::null,   state_actions(   a_null,   a_null, a_null   )),
        entry(states::red,    state_actions(    e_red,   a_null, l_red    )),
        entry(states::yellow, state_actions( e_yellow,   a_null, l_yellow )),
        entry(states::green,  state_actions(  e_green,   a_null, l_green  ))
      )
    );

  states  current = states::null;
  
  for (;;)
  {
    systick_timer::poll(
      0, // omit this parameter for systick or other hardware timers
      [&]() 
      {
        current = m(current);

        // Functionally identical implementation of state machine with 
        // switch for comparison
        /*
        switch(current)
        {
        default: // states::null
          current = states::red;
          e_red();
          break;
        case states::red:
          if (g_timeout())
          {
            l_red();
            current = states::yellow;
            e_yellow();
          }
          break;
        case states::yellow:
          if (g_timeout())
          {
            l_yellow();
            current = states::green;
            e_green();
          }
          break;
        case states::green:
          if (g_timeout())
          {
            l_green();
            current = states::red;
            e_red();
          }
          break;
        }*/
        
        return false;
      }
    );
  }
  
  return 0;
}
