#pragma once

#include <stdlib.h>
#include <BlocksAllocator.h>

namespace common {

template <typename T>
class StlBlockAllocator : public std::allocator<T> {
public:
    using Base = std::allocator<T>;
	using typename Base::size_type;
	using typename Base::difference_type;
	using typename Base::pointer;
	using typename Base::const_pointer;
	using typename Base::reference;
	using typename Base::const_reference;
	using typename Base::value_type;

    using Base::Base; // Just use constructors from base class

    template <class U>
    struct rebind {
        typedef StlBlockAllocator<U> other;
    };

    pointer allocate( size_type n, const void* = NULL ) {
        return static_cast<pointer>( common::blocks_allocator::allocate( n * sizeof( T ) ) );
    }

    void deallocate( pointer ptr, size_type n ) {
        common::blocks_allocator::deallocate( static_cast<void*>( ptr ) );
    }
};

template <typename T>
bool operator==( const common::StlBlockAllocator<T>& left, const common::StlBlockAllocator<T>& right ) {
    return &left == &right;
}
template <typename T>
bool operator!=( const common::StlBlockAllocator<T>& left, const common::StlBlockAllocator<T>& right ) {
    return &left != &right;
}

}  // namespace common

