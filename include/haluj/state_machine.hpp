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

#include <utility>

namespace haluj
{

struct null_action_t
{

  template<typename... Args>
  void operator()(Args...) const
  {}

};

null_action_t  a_null;

struct g_always_t
{
  template<typename... Args>
  bool operator()(Args...) const
  {
    return true;
  }
};

g_always_t g_always;

template <typename    EnterActionType = null_action_t,
          typename    DoActionType    = null_action_t,
          typename    ExitActionType  = null_action_t>
struct state_actions_t
{
  typedef EnterActionType   enter_action_type;
  typedef DoActionType      do_action_type;
  typedef ExitActionType    exit_action_type;
  
  state_actions_t(enter_action_type  p_enter_action = EnterActionType(),
                  do_action_type     p_do_action    = DoActionType(),
                  exit_action_type   p_exit_action  = ExitActionType())
  : enter_action(p_enter_action),
    do_action(p_do_action),
    exit_action(p_exit_action)
  {}
  
  const enter_action_type   enter_action;
  const do_action_type      do_action;
  const exit_action_type    exit_action;
};

template <typename    EnterActionType = null_action_t,
          typename    DoActionType    = null_action_t,
          typename    ExitActionType  = null_action_t>
constexpr state_actions_t<EnterActionType,
                          DoActionType,
                          ExitActionType> 
state_actions(const EnterActionType   p_enter_action  = EnterActionType(),
              const DoActionType      p_do_action     = DoActionType(),
              const ExitActionType    p_exit_action   = ExitActionType())
{
  return state_actions_t<EnterActionType, 
                         DoActionType, 
                         ExitActionType>(p_enter_action, 
                                         p_do_action, 
                                         p_exit_action);
}

/// generic transition template
template <typename    TriggerType,
          typename    StateType,
          typename    ActionType>
struct transition_t
{
  typedef TriggerType trigger_type;
  typedef StateType   state_type;
  typedef ActionType  action_type;

  transition_t( trigger_type p_trigger  = trigger_type(), 
                state_type   p_from     = state_type(), 
                state_type   p_to       = state_type(),
                action_type  p_action   = action_type())
  : trigger(p_trigger),
    from(p_from),
    to(p_to),
    action(p_action)
  {}
  
  template<typename... Args>
  bool test(const state_type p_current,
            Args...          args) const
  {
    return (p_current == from) && trigger(args...);
  }
  
  template<typename StateActionMap, 
           typename... Args>
  state_type do_transition(const StateActionMap&  p_map, 
                           Args&&...              args) const
  {
    p_map.exit_(from, std::forward<Args>(args)...);

    action(args...);

    p_map.enter_(to, std::forward<Args>(args)...);
    
    return to;
  }

  const trigger_type  trigger;
  const state_type    from; 
  const state_type    to;
  const action_type   action;
};


template <typename    TriggerType,
          typename    StateType,
          typename    ActionType  = null_action_t>
constexpr transition_t<TriggerType, StateType, ActionType>
transition(TriggerType  trigger, 
           StateType    from, 
           StateType    to, 
           ActionType   action = ActionType())
{
  return 
    transition_t<TriggerType, StateType, ActionType>
      (trigger, from, to, action);
}

/// generic empty graph template for variadic template final nesting
template <typename... Entries>
struct graph_t
{

  template <typename StateType, 
            typename StateActionMap,
            typename... Args>
  StateType operator()(const StateType          p_current, 
                       const StateActionMap&    p_map,
                       Args...                  args) const
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

  graph_t(const EdgeType& p_edge, Entries... args)
  : edge_(p_edge), next_(args...)
  {}

  template <typename StateType, 
            typename StateActionMap,
            typename... Args>
  StateType operator()(const StateType          p_current, 
                       const StateActionMap&    p_map,
                       Args&&...                args) const
  {
    if (edge_.test(p_current, std::forward<Args>(args)...))
    {
      return edge_.do_transition(p_map, std::forward<Args>(args)...);
    }

    return next_(p_current, p_map, std::forward<Args>(args)...);
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

template <typename    KeyType,
          typename    ValueType>
struct entry_t
{
  typedef KeyType     key_type;
  typedef ValueType   value_type;

  entry_t(const key_type&    p_key,
          const value_type&  p_value)
  : key (p_key),
    value (p_value)
  {}

  const key_type    key;
  const value_type  value;
};

template <typename    KeyType,
          typename    ValueType>
constexpr entry_t<KeyType, ValueType> 
entry(const KeyType& p_key, const ValueType& p_value)
{
  return entry_t<KeyType, ValueType>(p_key, p_value);
}

/// generic empty map template for variadic template final nesting
template <typename... Entries>
struct map_t
{
  template <typename    KeyType, typename... Args>  
  void enter_(const KeyType&, Args...) const
  {}

  template <typename    KeyType, typename... Args>  
  void do_(const KeyType&, Args...) const
  {}

  template <typename    KeyType, typename... Args>  
  void exit_(const KeyType&, Args...) const
  {}

};

/// generic map template for variadic template packing
template <typename    EntryType,
          typename... Entries>
struct map_t<EntryType, Entries...>
{
  typedef map_t<Entries...>                next;
  typedef EntryType                        entry_type;
  
  map_t(const entry_type& p_pair, Entries... args)
  : pair_(p_pair), next_(args...)
  {}

  // enter_ and exit_ functions are used in transition_t::do_transition 
  // method
  template <typename KeyType, typename... Args>  
  void enter_(const KeyType& p_key, Args&&... args) const
  {
    if (p_key == pair_.key)
    {
      pair_.value.enter_action(std::forward<Args>(args)...);
    }
    else
    {
      next_.enter_(p_key, std::forward<Args>(args)...);
    }
  }

  template <typename KeyType, typename... Args>  
  void do_(const KeyType& p_key, Args&&... args) const
  {
    if (p_key == pair_.key)
    {
      pair_.value.do_action(std::forward<Args>(args)...);
    }
    else
    {
      next_.do_(p_key, std::forward<Args>(args)...);
    }
  }

  template <typename KeyType, typename... Args>  
  void exit_(const KeyType& p_key, Args&&... args) const
  {
    if (p_key == pair_.key)
    {
      pair_.value.exit_action(std::forward<Args>(args)...);
    }
    else
    {
      next_.exit_(p_key, std::forward<Args>(args)...);
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
map_t<EntryType, Entries...>
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
  
  machine_t(const GraphType&  p_graph, 
            const MapType&    p_map)
  : graph_(p_graph),
    map_(p_map)
  {}

  template<typename StateType, typename... Args>
  StateType operator()(const StateType p_current, Args&&... args) 
  {
    map_.do_(p_current, args...);
    return graph_(p_current, map_, std::forward<Args>(args)...);
  }
  
  const graph_type        graph_;
  const map_type          map_;
};


template <typename GraphType,
          typename MapType>
machine_t<GraphType, MapType>
machine(const GraphType   &p_graph, 
        const MapType     &p_map)
{
  return 
    machine_t<GraphType, MapType>
      (p_graph, p_map);
    
}

} // namespace haluj

#endif //  HALUJ_STATE_MACHINE_HPP
