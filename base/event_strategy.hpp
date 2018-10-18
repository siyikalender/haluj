/// \file event_strategy.hpp
/// Primitive event mechanism implementation.
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

#ifndef HALUJ_BASE_EVENT_STRATEGY_HPP
#define HALUJ_BASE_EVENT_STRATEGY_HPP

namespace haluj
{
  
namespace base
{

template <typename EventType, 
          typename EventStrategyType>
struct on_t
{
  typedef EventType                     event_type;
  typedef EventStrategyType             event_strategy_type;
  
  on_t(const event_type             p_event = event_type(), 
       const event_strategy_type&   p_event_strategy = event_strategy_type())
  : event(p_event),
    event_strategy(p_event_strategy)
  {}
  
  bool operator()() const
  {
    return (event_strategy.test(event));
  }
  
  const event_type            event;
  const event_strategy_type&  event_strategy;
};

template <typename  EventType,
          typename  EventStrategyType>
constexpr on_t<EventType, EventStrategyType>
on(const EventType p_event, 
   const EventStrategyType& p_event_strategy)
{
  return 
    on_t<EventType, EventStrategyType>
      (p_event, p_event_strategy);
}


template <typename EventType>
struct blackboard_strategy
{
  bool test(EventType   p_event) const
  {
    return (is_valid && (pending_event == p_event));
  }

  void clear()
  {
    is_valid = false;
  }

  bool test_and_clear(EventType p_event)
  {
    bool result = false;
    
    if (test(p_event))
    {
      result = true;
      clear();
    }
    
    return result;
  }

  bool raise(EventType p_event)
  {
    bool result = false;
    if (!is_valid)
    {
      result = is_valid = true;
      pending_event     = p_event;
    }
    return result;
  }

  bool      is_valid = false;
  EventType pending_event;
};

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_EVENT_STRATEGY_HPP
