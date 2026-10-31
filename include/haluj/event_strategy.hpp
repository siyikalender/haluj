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

#ifndef HALUJ_EVENT_STRATEGY_HPP
#define HALUJ_EVENT_STRATEGY_HPP

#include "concurrency.hpp"

namespace haluj
{

template <typename EventType, 
          typename EventStrategyType>
struct on_t
{
  typedef EventType                     event_type;
  typedef EventStrategyType             event_strategy_type;
  
  /// The strategy is kept by reference, so it must outlive this object
  on_t(const event_type             p_event,
       const event_strategy_type&   p_event_strategy)
  : event(p_event),
    event_strategy(p_event_strategy)
  {}

  /// A temporary strategy would be destroyed before operator() runs
  on_t(const event_type, const event_strategy_type&&) = delete;
  
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

/// A temporary strategy would be destroyed before the result is used
template <typename  EventType,
          typename  EventStrategyType>
on_t<EventType, EventStrategyType>
on(const EventType, const EventStrategyType&&) = delete;

namespace strategy
{
  
/// A single slot for one pending event: raise() puts an event on the board
/// if it is free, test() looks at it and clear() takes it off.
///
/// Concurrency: single_context (the default) for a board used from one
/// context, or spsc when one context raises events and another tests and
/// clears them, for example an interrupt and the main loop. In spsc use the
/// producer calls only raise(), and the consumer only test(),
/// test_and_clear() and clear(). Only the producer turns the board from
/// free to taken and only the consumer turns it back, which is why plain
/// loads and stores are enough.
///
/// One producer only: two producers in different contexts, such as two
/// interrupts of different priority, could both see the board free and
/// both write it, and one event would overwrite the other unreported. That
/// needs an atomic read-modify-write, which Cortex-M0 does not have; there,
/// raise() from more than one context only with interrupts masked around
/// it.
template <typename EventType,
          typename Concurrency = single_context>
struct blackboard
{
  typedef EventType     event_type;
  typedef Concurrency   concurrency;

  /// consumer: true if p_event is pending
  bool test(EventType   p_event) const
  {
    // The flag is read first: once it is seen set, the event is there
    return (is_valid.load() && (pending_event == p_event));
  }

  /// consumer: takes the pending event off the board
  void clear()
  {
    is_valid.store(false);
  }

  /// consumer: takes p_event off the board if it is pending
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

  /// producer: puts p_event on the board if it is free; false if another
  /// event is still pending
  bool raise(EventType p_event)
  {
    bool result = false;

    if (!is_valid.load())
    {
      // The event first, then the flag that publishes it
      pending_event = p_event;
      is_valid.store(true);
      result = true;
    }

    return result;
  }

  shared<bool, Concurrency>  is_valid;
  EventType                  pending_event{};
};

} // namespace strategy

} // namespace haluj

#endif // HALUJ_EVENT_STRATEGY_HPP
