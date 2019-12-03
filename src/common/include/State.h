#pragma once

namespace common {
enum class StateSteps {
    CREATE,
    INIT,
    ENTER,
    RUN,
    LEAVE,
    FAULT,
    DESTROY,
};

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class StateSteps, class Event>
class State {
protected:
    const MachineStatesEnum machineStateID;
    StateSteps step = StateSteps::INIT;

public:
    explicit State( MachineStatesEnum machineStateID )
    : machineStateID( machineStateID ) {
        /*EMPTY*/
    }

    virtual ~State() {
        /*EMPTY*/
    }

    State( const State& other ) = delete;
    State( const State&& other ) = delete;
    State& operator=( const State& other ) = delete;

    virtual bool create() {
        return true;
    }
    virtual bool init() {
        return true;
    }
    virtual bool enter() {
        return true;
    }
    virtual bool run() {
        return true;
    }
    virtual bool leave() {
        return true;
    }
    virtual void destroy() {
        /*EMPTY*/
    }
    virtual void fault() {
        /*EMPTY*/
    }

    virtual bool dispatch( Event& /*e*/ ) {
        return true;
    }

    virtual bool doStep();
};

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class StateSteps, class Event>
inline bool State<MachineStatesEnum, StateSteps, Event>::doStep() {
    bool result = false;

    switch( step ) {
    case StateSteps::CREATE:
        result = create();
        step = StateSteps::INIT;
        break;
    case StateSteps::INIT:
        result = init();
        step = StateSteps::ENTER;
        break;
    case StateSteps::ENTER:
        result = enter();
        step = StateSteps::RUN;
        break;
    case StateSteps::RUN:
        result = run();
        break;
    case StateSteps::LEAVE:
        result = leave();
        step = StateSteps::ENTER;
        break;
    case StateSteps::FAULT:
        fault();
        result = true;
        break;
    case StateSteps::DESTROY:
        destroy();
        step = StateSteps::CREATE;
        result = true;
        break;
    }

    return result;
}

};  // namespace common
