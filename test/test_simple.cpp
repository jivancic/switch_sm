#include "../include/switch_sm.hpp"

#include <gtest/gtest.h>

struct State1 {};
struct State2 {};
struct State3 {};

struct Event1 {std::string data; };
struct Event2 {std::string data; };
struct Event3 {std::string data; };
struct Event4 {};

typedef std::tuple<State1, State2, State3> States;
typedef std::tuple<Event1, Event2, Event3, Event4> Events;

struct TransitionTable : public transition_table<States, Events>
{
    std::string const * operator()(int event_id, void * event_ptr)
    {
        transitions(event_id, event_ptr)
        {
            transition(State1, Event1, State2) { return &event.data; }
            transition(State2, Event1, State3) { return &event.data; }
            transition(State3, Event1, State1) { return &event.data; }
            transition(State1, Event2, State3) { return &event.data; }
            transition(State2, Event2, State1) { return &event.data; }
            transition(State3, Event2, State2) { return &event.data; }
            transition(State1, Event3, State1) { return &event.data; }
            transition(State2, Event3, State2) { return &event.data; }
            transition(State3, Event3, State3) { return &event.data; }
        }
        return nullptr;
    }
};

typedef state_machine<TransitionTable, State1> StateMachine;
Event1 event1 = {"event1"};
Event2 event2 = {"event2"};
Event3 event3 = {"event3"};
Event4 event4;

TEST(test_simple, test_transition) {
    StateMachine sm;
    sm.process_event(event1); EXPECT_TRUE(sm.current_state_is<State2>());
    sm.process_event(event1); EXPECT_TRUE(sm.current_state_is<State3>());
    sm.process_event(event1); EXPECT_TRUE(sm.current_state_is<State1>());
    
    sm.process_event(event2); EXPECT_TRUE(sm.current_state_is<State3>());
    sm.process_event(event2); EXPECT_TRUE(sm.current_state_is<State2>());
    sm.process_event(event2); EXPECT_TRUE(sm.current_state_is<State1>());
    
    sm.process_event(event3); EXPECT_TRUE(sm.current_state_is<State1>());
}

TEST(test_simple, test_result) {
    StateMachine sm;
    EXPECT_TRUE(sm.process_event(event1) == &event1.data);
    EXPECT_TRUE(sm.process_event(event2) == &event2.data);
    EXPECT_TRUE(sm.process_event(event3) == &event3.data);
    EXPECT_TRUE(sm.process_event(event4) == nullptr);
}

