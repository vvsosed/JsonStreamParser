
#pragma once

#include <stddef.h>

namespace common {

class IReadStream;

class Buffer {
public:
    using StorageType = void;

public:
    Buffer() = default;

    Buffer( const Buffer& ) = delete;

    Buffer( size_t size );

    Buffer( const char* str, size_t len = 0 );

    Buffer( Buffer&& other );

    Buffer( IReadStream& rstream, const size_t len );

    ~Buffer();

    operator bool() const;

    Buffer& operator=( const Buffer& ) = delete;

    Buffer& operator=( Buffer&& other );

    bool operator==( const Buffer& buf ) const;

    bool operator!=( const Buffer& buf ) const {
        return !( *this == buf );
    }

    size_t calculateMaxSize() const;

    bool realloc( size_t minSize );

    void reset();

    template <typename CastType = StorageType>
    inline CastType* get() {
        return reinterpret_cast<CastType*>( m_buffer );
    }

    template <typename CastType = StorageType>
    inline const CastType* get() const {
        return reinterpret_cast<const CastType*>( m_buffer );
    }

private:
    StorageType* m_buffer = nullptr;
};

#pragma GCC diagnostic ignored "-Wwrite-strings"

template <>
inline char* Buffer::get<char>() {
    static char* emptyStr = "";
    return m_buffer ? reinterpret_cast<char*>( m_buffer ) : emptyStr;
}

#pragma GCC diagnostic warning "-Wwrite-strings"

}  // namespace common
