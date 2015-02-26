#include "../include/switch_sm.hpp"

#include <chrono>
#include <iostream>

#define TEST_MSM 0

struct State1 : public state {};
struct State2 : public state {};
struct State3 : public state {};

struct Event1 {};
struct Event2 {};
struct Event3 {};
struct Event4 {};

typedef std::tuple<State1, State2, State3> States;
typedef std::tuple<Event1, Event2, Event3, Event4> Events;

struct TransitionTable : public transition_table<States, Events>
{
    char const * operator()(int event_id, void * event_ptr)
    {
        transitions(event_id, event_ptr)
        {
            on_event(State1, Event1) transit_to(State1) {
                static_assert(std::is_same<decltype(event), Event1 &>::value, "Invalid event type");
                return "State1 Event1";
            }
            on_event(State1, Event2) transit_to(State2) {
                static_assert(std::is_same<decltype(event), Event2 &>::value, "Invalid event type");
                return "State1 Event2";
            }
            on_event(State1, Event3) transit_to(State3) {
                static_assert(std::is_same<decltype(event), Event3 &>::value, "Invalid event type");
                return "State1 Event3";
            }
            on_event(State2, Event1) transit_to(State1) {
                static_assert(std::is_same<decltype(event), Event1 &>::value, "Invalid event type");
                return "State2 Event1";
            }
            on_event(State2, Event2) transit_to(State2) {
                static_assert(std::is_same<decltype(event), Event2 &>::value, "Invalid event type");
                return "State2 Event2";
            }
            on_event(State2, Event3) transit_to(State3) {
                static_assert(std::is_same<decltype(event), Event3 &>::value, "Invalid event type");
                return "State2 Event3";
            }
            on_event(State3, Event1) transit_to(State1) {
                static_assert(std::is_same<decltype(event), Event1 &>::value, "Invalid event type");
                return "State3 Event1";
            }
            on_event(State3, Event2) transit_to(State2) {
                static_assert(std::is_same<decltype(event), Event2 &>::value, "Invalid event type");
                return "State3 Event2";
            }
            on_event(State3, Event3) transit_to(State3) {
                static_assert(std::is_same<decltype(event), Event3 &>::value, "Invalid event type");
                return "State3 Event3";
            }
            on_no_match() { return "NO TRANSITION"; }
        }
        return "SHOULD NEVER HAPPEN";
    }
};

#if TEST_MSM
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace test_fsm // Concrete FSM implementation
{
    // events
    struct Event1 {};
    struct Event2 {};
    struct Event3 {};
    struct Event4 {};

    // Concrete FSM implementation 
    struct msm_sm_ : public msm::front::state_machine_def<msm_sm_>
    {
        // no need for exception handling or message queue
        typedef int no_exception_thrown;
        typedef int no_message_queue;

        struct State1 : public msm::front::state<> {};
        struct State2 : public msm::front::state<> {};
        struct State3 : public msm::front::state<> {};

        // the initial state of the player SM. Must be defined
        typedef State1 initial_state;

        typedef msm_sm_ m; // makes transition table cleaner
        
        template <typename Event>
        void do_something(Event const &) {
        }

        // Transition table for player
        struct transition_table : mpl::vector<
            //    Start     Event         Next      Action                 Guard
            //    +---------+-------------+---------+---------------------+----------------------+
              a_row < State1 , Event1      , State1, &m::do_something>,
              a_row < State1 , Event2      , State2, &m::do_something>,
              a_row < State1 , Event3      , State3, &m::do_something>,
              a_row < State2 , Event1      , State1, &m::do_something>,
              a_row < State2 , Event2      , State2, &m::do_something>,
              a_row < State2 , Event3      , State3, &m::do_something>,
              a_row < State3 , Event1      , State1, &m::do_something>,
              a_row < State3 , Event2      , State2, &m::do_something>,
              a_row < State3 , Event3      , State3, &m::do_something>
            //    +---------+-------------+---------+---------------------+----------------------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
        }
    };
    typedef msm::back::state_machine<msm_sm_> msm_sm;
}
#endif

int main()
{
    {
        Event1 event1;
        Event2 event2;
        Event3 event3;
        Event4 event4;

        StateMachine<TransitionTable, State1> sm;
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned int x(0); x < 100 * 1000 * 1000; ++x)
        {
            sm.process_event(event1);
            sm.process_event(event2);
            sm.process_event(event3);
            sm.process_event(event4);
        }

        std::cout << "Elapsed " <<
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start).count() << " microseconds"
            << '\n';
    }

#if TEST_MSM
    {
        test_fsm::Event1 event1;
        test_fsm::Event2 event2;
        test_fsm::Event3 event3;
        test_fsm::Event4 event4;
    
        test_fsm::msm_sm sm;
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned int x(0); x < 100 * 1000 * 1000; ++x)
        {
            sm.process_event(event1);
            sm.process_event(event2);
            sm.process_event(event3);
            sm.process_event(event4);
        }
    
        std::cout << "Elapsed " <<
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start).count() << " microseconds"
            << '\n';
    }
#endif
    
}
