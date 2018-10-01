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

#ifndef HALUJ_BASE_STATE_MACHINE_HPP
#define HALUJ_BASE_STATE_MACHINE_HPP

namespace haluj
{

namespace base
{

struct default_action
{

  template<typename... Args>
  void operator()(Args...) const
  {}

};

template <typename    EnterActionType = default_action,
          typename    DoActionType    = default_action,
          typename    ExitActionType  = default_action>
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

template <typename    EnterActionType = default_action,
          typename    DoActionType    = default_action,
          typename    ExitActionType  = default_action>
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
  
  bool test(const state_type p_current) const
  {
    return (p_current == from) && trigger();
  }
  
  template<typename StateActionMap>
  state_type do_transition(const StateActionMap& p_map) const
  {
    p_map.exit_(from);

    action();

    p_map.enter_(to);
    
    return to;
  }

  const trigger_type  trigger;
  const state_type    from; 
  const state_type    to;
  const action_type   action;
};


template <typename    TriggerType,
          typename    StateType,
          typename    ActionType  = default_action>
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
            typename StateActionMap>
  StateType operator()(const StateType          p_current, 
                       const StateActionMap&    p_map) const
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

  // Transition Operator
  template <typename StateType, 
            typename StateActionMap>
  StateType operator()(const StateType          p_current, 
                       const StateActionMap&    p_map) const
  {
    if (edge_.test(p_current))
    {
      return edge_.do_transition(p_map);
    }

    return next_(p_current, p_map);
  }

  edge_type     edge_;
  next          next_;
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

  key_type    key;
  value_type  value;
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
  
  template <typename    KeyType>  
  void enter_(const KeyType&) const
  {}

  template <typename    KeyType>  
  void do_(const KeyType&) const
  {}

  template <typename    KeyType>  
  void exit_(const KeyType&) const
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

  // Transition Operator
  template <typename KeyType>  
  void enter_(const KeyType& p_key) const
  {
    if (p_key == pair_.key)
    {
      pair_.value.enter_action();
    }
    else
    {
      next_.enter_(p_key);
    }
  }

  template <typename KeyType>  
  void do_(const KeyType& p_key) const
  {
    if (p_key == pair_.key)
    {
      pair_.value.do_action();
    }
    else
    {
      next_.do_(p_key);
    }
  }

  template <typename KeyType>  
  void exit_(const KeyType& p_key) const
  {
    if (p_key == pair_.key)
    {
      pair_.value.exit_action();
    }
    else
    {
      next_.exit_(p_key);
    }
  }
  
  const entry_type  pair_;
  next              next_;
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
template <typename StateType,
          typename GraphType,
          typename MapType>
struct machine_t
{
  typedef     StateType     state_type;
  typedef     GraphType     graph_type;
  typedef     MapType       map_type;
  
  machine_t(const StateType   p_null,
            const GraphType&  p_graph, 
            const MapType&    p_map)
  : current_(p_null),
    graph_(p_graph),
    map_(p_map),
    null_state_(p_null)
  {}

//  machine_t(const machine_t&  p_other)
//  : current_(p_other.current_),
//    graph_(p_other.graph_),
//    map_(p_other.map_),
//    null_state_(p_other.null_state_)
//  {}

  void initiate(const state_type  p_initial_state)
  {
    current_ = p_initial_state;
    map_.enter_(current_);
  }
  
  void proceed() 
  {
    map_.do_(current_);
    current_ = graph_(current_, map_); // test and do the transition 
    // Event life time should be only one tick long.
    // clear event if not processed.
    // event_strategy.clear(); // TO-DO clear() -> remove() or pop()
  }
  
  void terminate()
  {
    map_.exit_(current_);
    current_ = null_state_;
  }
  
  state_type              current_;
  graph_type              graph_;
  map_type                map_;
  const state_type        null_state_;
};


template <typename StateType,
          typename GraphType,
          typename MapType>
machine_t<StateType, GraphType, MapType>
machine(const StateType   p_null_state,
        const GraphType   &p_graph, 
        const MapType     &p_map)
{
  return 
    machine_t<StateType, GraphType, MapType>
      (p_null_state, p_graph, p_map);
    
}

} // namespace base

} // namespace haluj

#endif //  HALUJ_BASE_STATE_MACHINE_HPP
