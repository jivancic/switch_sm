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
        sm_ref(state_machine_base & sm_) : sm(sm_), was_executed(false) {}
        ~sm_ref() {
            if (was_executed)
                sm.complete_transition();
        }
        
        void start_transition(int next_state) {
            sm.target_state = next_state;
        }

        int state() const { return sm.state; }
        
        bool executed_check()
        {
            if (was_executed)
                return true;
            was_executed = true;
            return false;
        }
        
        state_machine_base & sm;
        bool was_executed;
    };
} // namespace detail
    
#define transitions(event_id, event_ptr) \
    detail::sm_ref _sm_ref(*sm_); \
    void * _ev_ptr = event_ptr; \
    switch (_sm_ref.state() | (event_id << 15)) \
        if (false) \
        { \
            break_out: \
            break;\
        } \
        else case 0:

#define break_preamble \
    for (;;) if (true) goto break_out; else
        
#define on_event(S, E) \
    break_preamble \
        case ((detail::index_of<S, States>::value) | \
                ((detail::index_of<E, Events>::value + 1) << 15)): \
            for (E & event(*static_cast<E *>(_ev_ptr));; ) \
                if (_sm_ref.executed_check()) \
                    goto break_out; \
                else

#define transit_to(T) \
    if (_sm_ref.start_transition(detail::index_of<T, States>::value), true)

#define on_no_match() \
    break_preamble \
        default: \
            for (;;) \
            if (_sm_ref.executed_check()) \
                goto break_out; \
            else

template <typename States, typename Events>
struct transition_table
{
    typedef States States;
    typedef Events Events;

    transition_table() : sm_(0) {}

    void set_state_machine(detail::state_machine_base & sm)
    {
        sm_ = &sm;
    }
    
    detail::state_machine_base * sm_;
};
        
template <typename TransitionTable, typename InitialState>
struct state_machine : public detail::state_machine_base
{
    typedef typename TransitionTable::States States;
    typedef typename TransitionTable::Events Events;
    TransitionTable transition_table;

    template <typename... Args>
    state_machine(Args&&... args) :
        detail::state_machine_base(detail::index_of<InitialState, States>::value),
        transition_table(std::forward<Args>(args)...)
    {
        transition_table.set_state_machine(*this);
    }

    template <typename Event>
    typename std::result_of<TransitionTable(int, void *)>::type
        process_event(Event & event)
    {
        return transition_table(detail::index_of<Event, Events>::value + 1,
            &event);
    }

    template <typename State>
    static int state_id() {
        return detail::index_of<State, States>::value;
    }

    template <typename State>
    bool current_state_is() const {
        return state == state_id<State>();
    }
};


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
