#pragma once

#include <stdint.h>
#include <assert.h>

#include <PointerArray.h>

namespace common {
template <class MachineStatesEnum, class State, uint32_t N>
class StatesTable : public PointerArray<State, N> {
public:
    explicit StatesTable()
    : PointerArray<State, N>() {
        /* EMPTY */
    }

    StatesTable( const StatesTable& other ) = delete;
    StatesTable( const StatesTable&& other ) = delete;
    StatesTable& operator=( const StatesTable& other ) = delete;

    const State* get( MachineStatesEnum i ) const;
    State* get( MachineStatesEnum i );
};

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class State, uint32_t N>
const State* StatesTable<MachineStatesEnum, State, N>::get( MachineStatesEnum i ) const {
    assert( i < MachineStatesEnum( N ) );
    return ( *this )[static_cast<uint32_t>( i )];
}

//-----------------------------------------------------------------------------

template <class MachineStatesEnum, class State, uint32_t N>
State* StatesTable<MachineStatesEnum, State, N>::get( MachineStatesEnum i ) {
    assert( i < MachineStatesEnum( N ) );
    return ( *this )[static_cast<uint32_t>( i )];
}

};  // namespace common
