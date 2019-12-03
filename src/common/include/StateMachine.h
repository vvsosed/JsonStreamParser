#pragma once

#include <assert.h>
#include <type_traits>

#include <State.h>

namespace common {
template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
class StateMachine {
    MachineStatesEnum currentState = MachineStatesEnum::INIT;
    MachineStatesTable* statesTable;
    const TransitionsTable* transitionsTable;

public:
    StateMachine( MachineStatesTable* states, TransitionsTable* transitions );

    ~StateMachine();

    StateMachine( const StateMachine& other ) = delete;
    StateMachine( const StateMachine&& other ) = delete;
    StateMachine& operator=( const StateMachine& other ) = delete;

    MachineStatesEnum getCurrentState();

    bool create();
    bool goState( MachineStatesEnum nextState );

    bool doStep();

    bool doDispatch( Event& e, MachineStatesEnum state = getCurrentState() );
};

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
StateMachine<MachineStatesEnum, MachineStatesTable, Event, Signal, TransitionsTable>::StateMachine(
    MachineStatesTable* states,
    TransitionsTable* transitions )
: statesTable( states )
, transitionsTable( transitions ) {
    static_assert( std::is_enum<MachineStatesEnum>::value, "MachineStatesEnum shall be enumeration" );

    assert( states != nullptr );
    assert( transitions != nullptr );
}

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
StateMachine<MachineStatesEnum, MachineStatesTable, Event, Signal, TransitionsTable>::~StateMachine() {
    statesTable->get( currentState )->leave();

    for( auto i = 0U; i < statesTable->size(); ++i ) {
        statesTable->get( MachineStatesEnum( i ) )->destroy();
    }
}

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
inline MachineStatesEnum
StateMachine<MachineStatesEnum, MachineStatesTable, Event, Signal, TransitionsTable>::getCurrentState() {
    return currentState;
}

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
bool StateMachine<MachineStatesEnum, MachineStatesTable, Event, Signal, TransitionsTable>::create() {
    bool result = true;
    for( auto i = 0U; i < statesTable->size(); ++i ) {
        result = statesTable->get( MachineStatesEnum( i ) )->create() && result;
    }

    return result;
}

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
bool StateMachine<MachineStatesEnum, MachineStatesTable, Event, Signal, TransitionsTable>::goState(
    MachineStatesEnum nextState ) {
    if( !transitionsTable->isTransitionValid( currentState, nextState ) ) {
        return false;
    }

    bool result = true;

    result = statesTable->get( currentState )->leave() && result;
    currentState = nextState;
    result = statesTable->get( currentState )->enter() && result;

    return result;
}

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
bool StateMachine<MachineStatesEnum, MachineStatesTable, Event, Signal, TransitionsTable>::doStep() {
    return statesTable->get( currentState )->doStep();
}

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class MachineStatesTable, class Event, class Signal, class TransitionsTable>
bool StateMachine<MachineStatesEnum, MachineStatesTable, Event, Signal, TransitionsTable>::doDispatch(
    Event& e,
    MachineStatesEnum state ) {
    if( state < MachineStatesEnum::COUNT ) {
        return statesTable->get( state )->dispatch( e );
    }

    return false;
}

};  // namespace common
