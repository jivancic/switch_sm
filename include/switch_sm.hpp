#include <type_traits>

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
        else case ((S::id) | (E::id << 15)): \
            for (E & event(*static_cast<E *>(_ev_ptr));; ) \
                if (_sm_ref.in_transition) \
                    goto break_out; \
                else if (_sm_ref.exit_state(T::id), true)

#define if_no_transition() \
    for (;;) \
        if (true) \
            goto break_out; \
        else default:

template <typename Func>
struct state_machine : public state_machine_base
{
    Func func;

    template <typename... Args>
    state_machine(Func && f, Args&&... args) :
        func(f),
        state_machine_base(std::forward<Args>(args)...)
    {
    }

    template <typename Event>
    typename std::result_of<Func(state_machine_base &, int, void *)>::type process_event(Event & event)
    {
        return func(*this, Event::id, &event);
    }
};

template <typename Func, typename... Args>
state_machine<Func> make_state_machine(Func && f, Args &&... args)
{
    return state_machine<Func>(f, std::forward<Args>(args)...);
}