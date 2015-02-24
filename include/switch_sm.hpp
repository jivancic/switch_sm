// Copyright (c) 2015
// Juraj Ivančić
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost/org/LICENSE_1_0.txt)
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#ifndef switch_sm_HPP__3E5B04CB_54D4_4334_A529_A5C31A793E23
#define switch_sm_HPP__3E5B04CB_54D4_4334_A529_A5C31A793E23
//------------------------------------------------------------------------------
#include <type_traits>
#include <tuple>
//------------------------------------------------------------------------------

namespace detail {
    template <typename T, typename Tuple>
    struct index_of;

    template <typename T, class... Types>
    struct index_of<T, std::tuple<T, Types...>> {
        static const std::size_t value = 0;
    };

    template <typename T, typename U, class... Types>
    struct index_of<T, std::tuple<U, Types...>> {
        typedef index_of<T, std::tuple<Types...>> next_index;
        static_assert(!std::is_same<std::tuple<Types...>, std::tuple<>>::value,
            "Type not found in tuple.");
        static const std::size_t value = 1 + next_index::value;
    };
};

struct state_machine_base
{
    explicit state_machine_base(int initial_state)
        : state(initial_state), target_state(-1)
    {}

    void complete_transition()
    {
        if (in_transition())
        {
            state = target_state;
            target_state = -1;
        }
    }

    bool in_transition() const {
        return target_state != -1;
    }

    int state;
    int target_state;
};

struct sm_ref
{
    sm_ref(state_machine_base & sm_) : sm(sm_), in_transition(false) {}
    ~sm_ref() {
        if (in_transition)
            sm.complete_transition();
    }
    
    void exit_state(int next_state) {
        sm.target_state = next_state;
        in_transition = true;
    }

    int state() const { return sm.state; }
    
    state_machine_base & sm;
    bool in_transition;
};
    
#define transitions(sm, event_id, event_ptr) \
    sm_ref _sm_ref(sm); \
    void * _ev_ptr = event_ptr; \
    switch (_sm_ref.state() | (event_id << 15)) \
        if (false) \
        { \
            break_out: \
            break;\
        } \
        else case 0:

#define transition(S, E, T) \
    for (;;) \
        if (true) \
        { \
            goto break_out; \
        } \
        else case ((detail::index_of<S, States>::value) | ((detail::index_of<E, Events>::value + 1) << 15)): \
            for (E & event(*static_cast<E *>(_ev_ptr));; ) \
                if (_sm_ref.in_transition) \
                    goto break_out; \
                else if (_sm_ref.exit_state(detail::index_of<T, States>::value), true)

#define if_no_transition() \
    for (;;) \
        if (true) \
            goto break_out; \
        else default:

template <typename TransitionTable, typename InitialState>
struct state_machine : public state_machine_base
{
    typedef typename TransitionTable::States States;
    typedef typename TransitionTable::Events Events;
    TransitionTable transition_table;

    state_machine() :
        state_machine_base(detail::index_of<InitialState, States>::value) {}

    template <typename Event>
    typename std::result_of<TransitionTable(state_machine_base &, int, void *)>::type
        process_event(Event & event)
    {
        return transition_table(*this, detail::index_of<Event, Events>::value + 1, &event);
    }
};


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
