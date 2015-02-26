#include "../include/switch_sm.hpp"

#include <gtest/gtest.h>

struct State1 : public state {};
struct State2 : public state {};
struct State3 : public state {};

struct Event1 {std::string data; };
struct Event2 {std::string data; };
struct Event3 {std::string data; };
struct Event4 {bool condition; };

typedef std::tuple<State1, State2, State3> States;
typedef std::tuple<Event1, Event2, Event3, Event4> Events;

struct ATransitionTable : public TransitionTable<States, Events>
{
    std::string const * operator()(int event_id, void * event_ptr)
    {
        transitions(event_id, event_ptr)
        {
            on_event(State1, Event1) transit_to(State2) { return &event.data; }
            on_event(State2, Event1) transit_to(State3) { return &event.data; }
            on_event(State3, Event1) transit_to(State1) { return &event.data; }
            on_event(State1, Event2) transit_to(State3) { return &event.data; }
            on_event(State2, Event2) transit_to(State1) { return &event.data; }
            on_event(State3, Event2) transit_to(State2) { return &event.data; }
            on_event(State1, Event3) { return &event.data; }
            on_event(State2, Event3) { return &event.data; }
            on_event(State3, Event3) { return &event.data; }
        }
        return nullptr;
    }
};

typedef StateMachine<ATransitionTable, State1> TestStateMachine;
Event1 event1 = {"event1"};
Event2 event2 = {"event2"};
Event3 event3 = {"event3"};
Event4 event4;

TEST(test_simple, test_transition) {
    TestStateMachine sm;
    EXPECT_TRUE(sm.current_state_is<State1>());
    sm.process_event(event1); EXPECT_TRUE(sm.current_state_is<State2>());
    sm.process_event(event1); EXPECT_TRUE(sm.current_state_is<State3>());
    sm.process_event(event1); EXPECT_TRUE(sm.current_state_is<State1>());
    
    sm.process_event(event2); EXPECT_TRUE(sm.current_state_is<State3>());
    sm.process_event(event2); EXPECT_TRUE(sm.current_state_is<State2>());
    sm.process_event(event2); EXPECT_TRUE(sm.current_state_is<State1>());
    
    sm.process_event(event3); EXPECT_TRUE(sm.current_state_is<State1>());
    sm.process_event(event1); EXPECT_TRUE(sm.current_state_is<State2>());
}

TEST(test_simple, test_result) {
    TestStateMachine sm;
    EXPECT_TRUE(sm.process_event(event1) == &event1.data);
    EXPECT_TRUE(sm.process_event(event2) == &event2.data);
    EXPECT_TRUE(sm.process_event(event3) == &event3.data);
    EXPECT_TRUE(sm.process_event(event4) == nullptr);
}

