#pragma once

#include <stdlib.h>
#include <BlocksAllocator.h>

namespace common {

template <typename T>
class StlBlockAllocator {
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    StlBlockAllocator() = default;
    StlBlockAllocator( const StlBlockAllocator<T>& ) = default;
    ~StlBlockAllocator() = default;

    template <class U>
    StlBlockAllocator( const StlBlockAllocator<U>& ){};
    template <class U>
    struct rebind {
        typedef StlBlockAllocator<U> other;
    };

    pointer address( reference ref ) const {
        return &ref;
    }

    const_pointer address( const_reference ref ) const {
        return &ref;
    }

    pointer allocate( size_type n, const void* = NULL ) {
        return static_cast<pointer>( common::blocks_allocator::allocate( n * sizeof( T ) ) );
    }

    void deallocate( pointer ptr, size_type n ) {
        common::blocks_allocator::deallocate( static_cast<void*>( ptr ) );
    }

    template <typename _Up, typename... _Args>
    void construct( _Up* ptr, _Args&&... args ) {
        ::new( static_cast<void*>( ptr ) ) _Up( std::forward<_Args>( args )... );
    }

    template <typename _Up>
    void destroy( _Up* ptr ) {
        ptr->~_Up();
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

