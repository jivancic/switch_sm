# Switch SM

Simple utility for generating fast (both at runtime and compile time), readable
state machines.

## Pre-requisites

Tested with `Visual Studio 2013`, but should work with any `C++11` compliant
compiler.

## Usage

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
    
Now define a transition table. It should look something like this:

``` cpp
    struct TransitionTable
    {
        typedef std::tuple<Sleeping, Coding> States;
        typedef std::tuple<WakeUp, Tired> Events;
        
        void operator()(state_machine_base & sm, int event_id, void * event_data)
        {
            transitions(sm, event_id, event_data)
            {
                transition(Sleeping, WakeUp, Coding) {
                    std::cout << "Coder wakes up and starts coding.\n";
                }
                transition(Sleeping, Tired, Sleeping) {
                    std::cout << "Coder is already sleeping.\n";
                }
                transition(Coding, Tired, Sleeping) {
                    std::cout << "Coder is tired and goes to sleep.\n";
                }
                transition(Coding, WakeUp, Coding) {
                    std::cout << "Coder is already coding.\n";
                }
                if_no_transition() {
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
        state_machine<TransitionTable, Sleeping> sm;
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
    Already coding.
    Coder is tired and goes to sleep.
    Already sleeping.
```

### Accessing event data

Each transition(s) (compound) statement has access to an `event` variable.
This variable is of the correct type, and can be used to access event specific
data.

``` cpp
    void operator()(state_machine_base & sm, int event_id, void * event_data)
    {
        transitions(sm, event_id, event_data)
        {
            transition(Sleeping, WakeUp, Coding) {
                static_assert(std::is_same<decltype(event), WakeUp &>::value,
                "Invalid event type");
            }
            transition(Sleeping, Tired, Sleeping) {
                static_assert(std::is_same<decltype(event), Tired &>::value,
                "Invalid event type");
            }
            transition(Coding, Tired, Sleeping) {
                static_assert(std::is_same<decltype(event), Tired &>::value,
                "Invalid event type");
            }
            transition(Coding, WakeUp, Coding) {
                static_assert(std::is_same<decltype(event), WakeUp &>::value,
                "Invalid event type");
            }
        }
    }
```

### Returning values from transition table

Transition table can return values, so the above code can be written more tersly:

``` cpp
    char const * transition_table(state_machine_base & sm,
        int event_id, void * event_data)
    {
        transitions(sm, event_id, event_data)
        {
            transition(Sleeping, WakeUp, Coding) {
                return "Coder wakes up and starts coding.\n";
            }
            transition(Sleeping, Tired , Sleeping) {
                return "Already sleeping.\n";
            }
            transition(Coding, Tired, Sleeping) {
                return "Coder is tired and goes to sleep.\n";
            }
            transition(Coding, WakeUp, Coding) {
                return "Already coding.\n";
            }
            if_no_transition() {
                return "Unexpected event.\n";
            }
        }
        return nullptr;
    }
```

State machine's `process_event` return value will match the return value of
`transition_table`:

``` cpp
        std::cout << coder.process_event(wakeUp);
        std::cout << coder.process_event(wakeUp);
        std::cout << coder.process_event(tired);
        std::cout << coder.process_event(tired);
```

## Performance

Switch SM is basically only syntactic sugar for generating `switch`/`case`
statement. State machine's double dispatch is performed by the `transition_table`.
If written as described above, it will be preprocessed into a single `switch`
statement and should provide **O**(1) execution speed.

Compared to a simple state machine written in Boost.MSM, Switch SM is ~30-40%
slower (tested on MSVC2013).
This is not bad considering that Boost.MSM is the fastest guy around when it
comes to runtime execution.

## Todo

This is a work in progress. I hope to add additional features later on.

#### Other

In case you have glanced at `switch_sm.hpp` and now are wondering what on earth
are those evil macros doing - have a look at Duff's machine and Boost.ASIO
stackless coroutines. The latter have inspired this little state machine
utility.



