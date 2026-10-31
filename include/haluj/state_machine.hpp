/// \file state_machine.hpp
/// a lightweight state machine implementation
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

#ifndef HALUJ_STATE_MACHINE_HPP
#define HALUJ_STATE_MACHINE_HPP

#include <cstddef>
#include <tuple>
#include <utility>

namespace haluj
{

// A state machine built at compile time:
//
//   auto m = machine(graph(transition<From, To>(guard, action), ...),
//                    map(entry<State>(std::make_tuple(enter, do, exit)), ...));
//
//   state = m(state, arguments...);
//
// A call to the machine, with any arguments, runs:
//   1. the do action of the current state
//   2. the guards of the transitions leaving the current state, in the
//      order they are written: the first that returns true is taken
//   3. if one is taken: the exit action of the current state, the action of
//      the transition and the entry action of the new state
// and returns the new state, or the current one if no guard held.
//
// A transition to the same state (From == To) runs exit and entry too.
// nullptr stands for a missing action, and a state without an entry in the
// map has no actions.
//
// Built from captureless lambdas or functions, the machine can be constexpr
// and placed in read only memory.

/// Always true guard
struct g_always_t
{
  template<typename... Args>
  constexpr bool operator()(Args&&...) const
  {
    return true;
  }
};

// The arguments of a call to the machine reach every guard and action of
// that call: the guards tested, then exit, transition action and enter. So
// they are never forwarded: an rvalue forwarded to the first of them could
// be moved from and reach the next ones empty. They are passed on as
// lvalues, and a callback may take them by value, by (const) reference or
// as auto&&, but not as a concrete rvalue reference such as std::string&&.

template < typename... Args >
constexpr void _invoke_a(std::nullptr_t, Args&&...)
{}

template < typename Action, typename... Args >
constexpr void _invoke_a(const Action& action, Args&&... args)
{
  action(args...);
}

constexpr g_always_t g_always;

/// generic transition template
template 
< 
  auto            From,
  decltype(From)  To,
  typename        TriggerType,
  typename        ActionType = std::nullptr_t
>
struct transition_t
{
  typedef TriggerType     trigger_type;
  typedef decltype(From)  state_type;
  typedef ActionType      action_type;
  
  static constexpr state_type from = From;
  static constexpr state_type to   = To;

  constexpr transition_t( trigger_type p_trigger,
                action_type  p_action   = nullptr)
  : trigger(p_trigger),
    action(p_action)
  {}
  
  template<typename... Args>
  constexpr bool test(const state_type p_current,
            Args&&...        args) const
  {
    return (p_current == from) && trigger(args...);
  }
  
  template<typename StateActionMap, 
           typename... Args>
  constexpr state_type do_transition(const StateActionMap&  p_map,
                           Args&&...              args) const
  {
    p_map.exit_(from, args...);

    _invoke_a(action, args...);

    p_map.enter_(to, args...);
    
    return to;
  }

  const trigger_type  trigger;
  const action_type   action;
};

template 
<
  auto            From,
  decltype(From)  To,
  typename        TriggerType,
  typename        ActionType  = std::nullptr_t
>
constexpr transition_t<From, To, TriggerType, ActionType>
transition(TriggerType  trigger, 
           ActionType   action = ActionType())
{
  return 
    transition_t<From, To, TriggerType, ActionType>
      (trigger, action);
}

/// generic empty graph template for variadic template final nesting
template <typename... Entries>
struct graph_t
{

  template <typename StateType, 
            typename StateActionMap,
            typename... Args>
  constexpr StateType operator()(const StateType          p_current,
                       const StateActionMap&,
                       Args&&...) const
  {
    return p_current;
  }
};

/// generic empty graph template for variadic template packing
template <typename EdgeType, typename... Entries>
struct graph_t<EdgeType, Entries...>
{
  typedef   graph_t<Entries...>    next;
  typedef   EdgeType               edge_type;

  constexpr graph_t(const EdgeType& p_edge, Entries... args)
  : edge_(p_edge), next_(args...)
  {}

  template <typename StateType, 
            typename StateActionMap,
            typename... Args>
  constexpr StateType operator()(const StateType          p_current,
                       const StateActionMap&    p_map,
                       Args&&...                args) const
  {
    if (edge_.test(p_current, args...))
    {
      return edge_.do_transition(p_map, args...);
    }

    return next_(p_current, p_map, args...);
  }

  const edge_type     edge_;
  const next          next_;
};

template<typename EdgeType>
constexpr graph_t<EdgeType>
graph(const EdgeType& p_edge)
{
  return graph_t<EdgeType>(p_edge);
}

template<typename EdgeType, typename ... Entries>
constexpr graph_t<EdgeType, Entries...>
graph(const EdgeType& p_a, Entries... args)
{
  return graph_t<EdgeType, Entries...>(p_a, args...);
}

template <auto        Key,
          typename    ValueType>
struct entry_t
{
  typedef decltype(Key)       key_type;
  typedef ValueType           value_type;
  
  static constexpr key_type   key = Key;

  constexpr entry_t(const value_type&  p_value)
  : value (p_value)
  {}

  const value_type  value;
};

template <auto        Key,
          typename    ValueType>
constexpr entry_t<Key, ValueType>
entry(const ValueType& p_value)
{
  return entry_t<Key, ValueType>(p_value);
}

/// generic empty map template for variadic template final nesting
template <typename... Entries>
struct map_t
{
  template <typename    KeyType, typename... Args>  
  constexpr void enter_(const KeyType&, Args&&...) const
  {}

  template <typename    KeyType, typename... Args>  
  constexpr void do_(const KeyType&, Args&&...) const
  {}

  template <typename    KeyType, typename... Args>  
  constexpr void exit_(const KeyType&, Args&&...) const
  {}

};

/// generic map template for variadic template packing
template <typename    EntryType,
          typename... Entries>
struct map_t<EntryType, Entries...>
{
  typedef map_t<Entries...>                next;
  typedef EntryType                        entry_type;
  
  constexpr map_t(const entry_type& p_pair, Entries... args)
  : pair_(p_pair), next_(args...)
  {}

  // enter_ and exit_ functions are used in transition_t::do_transition 
  // method
  template <typename KeyType, typename... Args>  
  constexpr void enter_(const KeyType& p_key, Args&&... args) const
  {
    if (p_key == entry_type::key)
    {
      _invoke_a(std::get<0>(pair_.value), args...);
    }
    else
    {
      next_.enter_(p_key, args...);
    }
  }

  template <typename KeyType, typename... Args>  
  constexpr void do_(const KeyType& p_key, Args&&... args) const
  {
    if (p_key == entry_type::key)
    {
      _invoke_a(std::get<1>(pair_.value), args...);
    }
    else
    {
      next_.do_(p_key, args...);
    }
  }

  template <typename KeyType, typename... Args>  
  constexpr void exit_(const KeyType& p_key, Args&&... args) const
  {
    if (p_key == entry_type::key)
    {
      _invoke_a(std::get<2>(pair_.value), args...);
    }
    else
    {
      next_.exit_(p_key, args...);
    }
  }
  
  const entry_type  pair_;
  const next        next_;
};

template<typename EntryType>
constexpr map_t<EntryType>
map(const EntryType& p_edge)
{
  return map_t<EntryType>(p_edge);
}

template<typename EntryType, typename ... Entries>
constexpr map_t<EntryType, Entries...>
map(const EntryType& p_a, Entries... args)
{
  return map_t<EntryType, Entries...>(p_a, args...);
}

/// core state machine
template <typename GraphType,
          typename MapType>
struct machine_t
{
  typedef     GraphType     graph_type;
  typedef     MapType       map_type;
  
  constexpr machine_t(const GraphType&  p_graph,
            const MapType&    p_map)
  : graph_(p_graph),
    map_(p_map)
  {}

  template<typename StateType, typename... Args>
  constexpr StateType operator()(const StateType p_current, Args&&... args) const
  {
    map_.do_(p_current, args...);
    return graph_(p_current, map_, args...);
  }
  
  const graph_type        graph_;
  const map_type          map_;
};

template <typename GraphType,
          typename MapType>
constexpr machine_t<GraphType, MapType>
machine(const GraphType   &p_graph, 
        const MapType     &p_map)
{
  return 
    machine_t<GraphType, MapType>
      (p_graph, p_map);
    
}

} // namespace haluj

#endif //  HALUJ_STATE_MACHINE_HPP
