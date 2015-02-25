# Switch SM

Simple utility for generating fast (both at runtime and compile time), readable
state machines.

## Pre-requisites

Tested with `Visual Studio 2013`, but should work with any `C++11` compliant
compiler.

## Basic usage

First of all, you need to include Switch SM header.

``` cpp
    #include <switch_sm.hpp>
```

Define `struct`s which correspond to states. States are compile-time entities
only and are (currently) never instantiated.

``` cpp
    struct Sleeping {};
    struct Coding {};
```

Next, define some events:

``` cpp
    struct WakeUp {};
    struct Tired {};
```
    
Wrap these into type lists:

``` cpp
    typedef std::tuple<Sleeping, Coding> States;
    typedef std::tuple<WakeUp, Tired> Events;
```
    
Now define a transition table. It should look something like this:

``` cpp
    struct CoderTransitions : public transition_table<States, Events>
    {
        void operator()(int event_id, void * event_data)
        {
            transitions(event_id, event_data)
            {
                on_event(Sleeping, WakeUp) transit_to(Coding) {
                    std::cout << "Coder wakes up and starts coding.\n";
                }
                on_event(Sleeping, Tired) {
                    std::cout << "Coder is already sleeping.\n";
                }
                on_event(Coding, Tired) transit_to(Sleeping) {
                    std::cout << "Coder is tired and goes to sleep.\n";
                }
                on_event(Coding, WakeUp) {
                    std::cout << "Coder is already coding.\n";
                }
                on_no_match() {
                    std::cout << "Unexpected event.\n";
                }
            }
        }
    };
```

All the ingredients are now in place.

``` cpp
    int main() {
        WakeUp wakeUp;
        Tired tired;
        
        // Second parameter represents initial state for the state machine.
        state_machine<CoderTransitions, Sleeping> coder;
        coder.process_event(wakeUp);
        coder.process_event(wakeUp);
        coder.process_event(tired);
        coder.process_event(tired);
        return 0;
    }
```

The output is:

``` console
    Coder wakes up and starts coding.
    Coder is already coding.
    Coder is tired and goes to sleep.
    Coder is already sleeping.
```

### Accessing event data

In each `on_event` (compound) statement, `event_data` is cast into appropriate
event type. This data can be used to access additional event information.

``` cpp
    void operator()(int event_id, void * event_data)
    {
        transitions(event_id, event_data)
        {
            on_event(Sleeping, WakeUp) {
                static_assert(std::is_same<decltype(event), WakeUp &>::value,
                "Invalid event type");
            }
            on_event(Sleeping, Tired, Sleeping) {
                static_assert(std::is_same<decltype(event), Tired &>::value,
                "Invalid event type");
            }
            // etc...
        }
    }
```

### Passing parameters to transition table

It is often useful to be able to modify an external resource when performing
transitions. You can construct your transition table using non-default
constructor by passing appropriate arguments to the state machine constructor:

``` cpp
        state_machine<Transitions, InitialState> coder(database);
```

In the above example, `database` parameter will be passed to the `Transitions`
constructor.

### Conditional transitions

If you want perform conditional transitions use the following syntax:

``` cpp
    on_event(State1, Event)
    {
        if (event.transit_to_state2)
            transit_to(State2) { std::cout << "Transition to State2.\n" }
        else
            transit_to(State3) { std::cout << "Transition to State3.\n" }
    }
```

This technique can also be used to implement guarded transitions.

### Returning values from transition table

Transition table can return values, so our `Coder` transition table can be written more tersly:

``` cpp
    char const * operator()(int event_id, void * event_data)
    {
        transitions(event_id, event_data)
        {
            on_event(Sleeping, WakeUp) transit_to(Coding) {
                return "Coder wakes up and starts coding.\n";
            }
            on_event(Sleeping, Tired) {
                return "Coder is already sleeping.\n";
            }
            on_event(Coding, Tired) transit_to(Sleeping) {
                return "Coder is tired and goes to sleep.\n";
            }
            on_event(Coding, WakeUp) {
                return "Coder is already coding.\n";
            }
            on_no_match() {
                return "Unexpected event.\n";
            }
        }
        return nullptr;
    }
```

State machine's `process_event` return value will match the return value of
`CoderTransitions::operator()`:

``` cpp
        std::cout << coder.process_event(wakeUp);
        std::cout << coder.process_event(wakeUp);
        std::cout << coder.process_event(tired);
        std::cout << coder.process_event(tired);
```

## Performance

Switch SM is basically only syntactic sugar for generating `switch`/`case`
statement. State machine's double dispatch is performed by the transition
table's `operator()`. If written as described above, it will be
preprocessed into a single `switch` statement and should provide **O**(1)
execution speed.

Compared to a simple state machine written in
[Boost.MSM](http://www.boost.org/doc/libs/release/libs/msm/doc/HTML/index.html),
Switch SM ~25% slower (tested on MSVC2013). This is not bad considering that
Boost.MSM is the fastest solution when it comes to SM runtime execution.

## Todo

* Add more tests.
* Add additional features.
    * State entry/exit events.
    * Event queue (deferral).

#### Other

In case you have glanced at `switch_sm.hpp` and now are wondering what on earth
are those evil macros doing - have a look at
[Duff's device](http://en.wikipedia.org/wiki/Duff%27s_device) and [Boost.ASIO
stackless coroutines](http://www.boost.org/doc/libs/release/doc/html/boost_asio/reference/coroutine.html).
The latter have inspired this little state machine utility.



