#include <Buffer.h>
#include <BlocksAllocator.h>
#include <IReadStream.h>

#include <cstring>
#include <algorithm>

namespace common {

Buffer::Buffer( size_t size )
: m_buffer( blocks_allocator::allocate( size ) ) {
    if( m_buffer ) {
        *( (char*)m_buffer ) = '\0';
    }
}

Buffer::Buffer( const char* str, size_t len ) {
    if( nullptr == str ) {
        return;
    }

    if( 0 == len ) {
        len = std::strlen( str );
        if( !len ) {
            return;
        }
    }

    m_buffer = blocks_allocator::allocate( len + 1 );
    if( !m_buffer ) {
        return;
    }

    memcpy( get<char>(), str, len );
    *( get<char>() + len ) = '\0';
}

Buffer::~Buffer() {
    reset();
}

Buffer::Buffer( Buffer&& other )
: m_buffer( other.m_buffer ) {
    other.m_buffer = nullptr;
}

Buffer::Buffer( IReadStream& rstream, const size_t len )
: m_buffer( blocks_allocator::allocate( len + 1 ) ) {
    if( !m_buffer ) {
        return;
    }

    char buf[128];
    size_t offset = 0;
    while( len > offset ) {
        auto bytesToRead = std::min( sizeof( buf ), len - offset );
        if( bytesToRead != rstream.read( buf, bytesToRead ) ) {
            break;
        }
        std::memcpy( m_buffer, buf + offset, bytesToRead );
        offset += bytesToRead;
    }

    static_cast<char*>( m_buffer )[offset] = '\0';
}

Buffer& Buffer::operator=( Buffer&& other ) {
    reset();

    m_buffer = other.m_buffer;
    other.m_buffer = nullptr;

    return *this;
}

bool Buffer::operator==( const Buffer& buf ) const {
    return m_buffer == buf.m_buffer;
}

Buffer::operator bool() const {
    return m_buffer != nullptr;
}

size_t Buffer::calculateMaxSize() const {
    if( m_buffer ) {
        return blocks_allocator::calculateRealSize( m_buffer );
    }
    return 0;
}

bool Buffer::realloc( const size_t minSize ) {
    auto newBuffer = blocks_allocator::reallocate( m_buffer, minSize );
    if( !newBuffer ) {
        return false;
    }

    m_buffer = newBuffer;
    return true;
}

void Buffer::reset() {
    if( m_buffer ) {
        blocks_allocator::deallocate( m_buffer );
    }
    m_buffer = nullptr;
}

}  // namespace common
