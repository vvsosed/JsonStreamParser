#pragma once

#include <stdint.h>
#include <PointerArray.h>

namespace common {
template <class StatesEnum>
struct Transition {
    StatesEnum from;
    StatesEnum to;
};

//-----------------------------------------------------------------------------

template <class StatesEnum, class Transition, uint32_t N>
class TransitionsTable : public PointerArray<Transition, N> {
    typedef PointerArray<Transition, N> TransitionsTableBase;

public:
    explicit TransitionsTable()
    : TransitionsTableBase() {
        static_assert( std::is_enum<StatesEnum>::value, "StatesEnum shall be enum" );
        static_assert( std::is_member_object_pointer<decltype( &Transition::from )>::value,
                       "Transition::from field is missing" );
        static_assert( std::is_member_object_pointer<decltype( &Transition::to )>::value,
                       "Transition::to field is missing" );
    }

    TransitionsTable( const TransitionsTable& other ) = delete;
    TransitionsTable( const TransitionsTable&& other ) = delete;
    TransitionsTable& operator=( const TransitionsTable& other ) = delete;

    inline bool copyFrom( Transition** buff, uint32_t n ) {
        uint32_t count = getTransitionsCount();
        assert( n == count );
        return TransitionsTableBase::copyFrom( buff, n );
    }

    constexpr uint32_t getTransitionsCount() {
        return N;
    }
    bool isTransitionValid( StatesEnum from, StatesEnum to ) const;
};

//-----------------------------------------------------------------------------

template <class StatesEnum, class Transition, uint32_t N>
bool TransitionsTable<StatesEnum, Transition, N>::isTransitionValid( StatesEnum from, StatesEnum to ) const {
    if( from >= StatesEnum::COUNT || to >= StatesEnum::COUNT ) {
        return false;
    }

    auto condition = [&from]( const Transition& t ) -> bool { return ( t->from == from ); };

    auto prevState = TransitionsTableBase::find( condition );

    while( prevState != TransitionsTableBase::end() ) {
        if( prevState->to == to ) {
            return true;
        }

        prevState = TransitionsTableBase::findNext( prevState + 1, condition );
    }

    return false;
}

};  // namespace common
