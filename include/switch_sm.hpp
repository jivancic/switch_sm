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
    template <typename, typename>
    struct IndexOf;

    template <typename T, class... Types>
    struct IndexOf<T, std::tuple<T, Types...>> {
        static const std::size_t value = 0;
    };

    template <typename T, typename U, class... Types>
    struct IndexOf<T, std::tuple<U, Types...>> {
        typedef IndexOf<T, std::tuple<Types...>> next_index;
        static_assert(!std::is_same<std::tuple<Types...>, std::tuple<>>::value,
            "Type not found in tuple.");
        static const std::size_t value = 1 + next_index::value;
    };

    template <typename Tuple, int Index = 0, typename Func, typename... Args>
    typename std::enable_if<Index == std::tuple_size<Tuple>::value, void>::type
        TupleSwitch(int, Func &&, Args&&...)
    {
    }

    template <typename Tuple, int Index = 0, typename Func, typename... Args>
    typename std::enable_if<Index < std::tuple_size<Tuple>::value, void>::type
        TupleSwitch(int i, Func && func, Args&&... args)
    {
        typedef typename std::tuple_element<Index, Tuple>::type TupleElem;
        if (Index == i)
            func(TupleElem(), std::forward<Args>(args)...);
        else
            TupleSwitch<Tuple, Index + 1>(i, std::forward<Func>(func),
                std::forward<Args>(args)...);
    }

    struct FalseOnce
    {
        FalseOnce() : was_false_(false) {}
        
        bool check()
        {
            if (was_false_)
                return true;
            was_false_ = true;
            return false;
        }
      
        bool was_false_;
    };

    template <typename SM, typename Event>
    struct TransitionHandler : FalseOnce
    {
        TransitionHandler(SM & sm, Event & event, int state)
            : sm_(sm), event_(event)
        {
            sm_.begin_transition(event_, state);
        }

        ~TransitionHandler()
        {
            sm_.end_transition(event_);
        }
        
        Event & event_;
        SM & sm_;
    };
    
    template <typename SM, typename Event>
    TransitionHandler<SM, Event> handle_transition(SM & sm, Event & event, int state)
    {
        return TransitionHandler<SM, Event>(sm, event, state);
    }
} // namespace detail
    
#define transitions(event_id, event_ptr) \
    detail::FalseOnce branch_taken; \
    void * _ev_ptr = event_ptr; \
    switch (sm_->state() | (event_id << 15)) \
        if (false) \
        { \
            break_out: \
            break;\
        } \
        else case -1:

#define break_preamble \
    for (;;) if (true) goto break_out; else
        
#define on_event(S, E) \
    break_preamble \
        case ((detail::IndexOf<S, States>::value) | \
                ((detail::IndexOf<E, Events>::value) << 15)): \
            for (E & event(*static_cast<E *>(_ev_ptr));; ) \
                if (branch_taken.check()) \
                    goto break_out; \
                else

#define transit_to(T) \
    for (auto t = detail::handle_transition(*sm_, event, \
            detail::IndexOf<T, States>::value); ; ) \
        if (t.check()) \
            goto break_out; \
        else

#define on_no_match() \
    break_preamble \
        default: \
            for (;;) \
            if (branch_taken.check()) \
                goto break_out; \
            else

struct state
{
    template <typename Event>
    inline static void on_entry(Event &) {}
    
    template <typename Event>
    inline static void on_exit(Event &) {}
};
                
template <typename TT>
class StateMachine;
                
template <typename Derived, typename States, typename Events>
struct TransitionTable
{
    typedef States States;
    typedef Events Events;

    TransitionTable() : sm_(0) {}

    void set_state_machine(StateMachine<Derived> & sm)
    {
        sm_ = &sm;
    }

    StateMachine<Derived> * sm_;
};
        
struct InitialEvent {};
        
template <typename TT>
class StateMachine
{
private:
    struct CallExit
    {
        template <typename State, typename Event>
        void operator()(State, Event & event)
        {
            State::on_exit(event);
        }
    };

    struct CallEntry
    {
        template <typename State, typename Event>
        void operator()(State, Event & event)
        {
            State::on_entry(event);
        }
    };

public:
    typedef TT TransitionTable;
    typedef typename TransitionTable::States States;
    typedef typename TransitionTable::Events Events;

    template <typename... Args>
    StateMachine(Args&&... args) :
        transition_table(std::forward<Args>(args)...),
        state_(-1), target_state_(-1)
    {
        transition_table.set_state_machine(*this);
    }
    
    template <typename InitialState, typename Event=InitialEvent>
    void start()
    {
        state_ = detail::IndexOf<InitialState, States>::value;
        Event event;
        call_entry(event);
    }

    template <typename Event>
    typename std::result_of<TransitionTable(int, void *)>::type
        process_event(Event & event)
    {
        return transition_table(detail::IndexOf<Event, Events>::value,
            &event);
    }

    template <typename State>
    static int state_id() {
        return detail::IndexOf<State, States>::value;
    }

    int state() const {
        return state_;
    }

    template <typename State>
    bool current_state_is() const {
        return state_ == state_id<State>();
    }

    template <typename Event>
    void begin_transition(Event & event, int state) {
        call_exit(event);
        target_state_ = state;
    }

    template <typename Event>
    void end_transition(Event & event) {
        state_ = target_state_;
        target_state_ = -1;
        call_entry(event);
    }

private:
    template <typename Event>
    void call_entry(Event & event)
    {
        detail::TupleSwitch<States>(state_, CallEntry(), event);
    }

    template <typename Event>
    void call_exit(Event & event)
    {
        detail::TupleSwitch<States>(state_, CallExit(), event);
    }

private:
    TT transition_table;
    int state_;
    int target_state_;
};


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
