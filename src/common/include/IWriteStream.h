
#pragma once

#include <cstddef>
#include <type_traits>
#include <string>
#include <cstring>
#include <list>
#include <unordered_map>
#include <set>
#include <memory>
#include "StlBlockTypes.h"

namespace common {

class IWriteStream {
    enum class SerializationMarker : std::uint8_t { Item, End };

public:
    using UPtr = std::unique_ptr<IWriteStream>;

    virtual ~IWriteStream() = default;

public:
    virtual std::size_t write( const char* buffer, std::size_t size ) = 0;

    template <typename ValueType,
              typename std::enable_if<std::is_integral<ValueType>::value || std::is_floating_point<ValueType>::value ||
                                          std::is_enum<ValueType>::value,
                                      int>::type = 0>
    inline bool write( const ValueType& value ) {
        return sizeof( ValueType ) == write( reinterpret_cast<const char*>( &value ), sizeof( ValueType ) );
    }

    inline bool write( const std::string& str ) {
        std::size_t lenght = str.length();
        return write( lenght ) && lenght == write( str.c_str(), lenght );
    }

    template <typename T>
    bool write( const std::list<T>& list ) {
        return writeListLikeContainer( list );
    }

    template <typename T>
    bool write( const block_list<T>& list ) {
        return writeListLikeContainer( list );
    }

    template <typename Key, typename Value>
    bool write( const std::unordered_map<Key, Value>& map ) {
        return writeMapLikeContainer( map );
    }

    template <typename Value>
    bool write( const std::set<Value>& set ) {
        return writeListLikeContainer( set );
    }

    template <typename T, typename std::enable_if<std::is_class<T>::value, int>::type = 0>
    inline bool write( const T& object ) {
        return object.write( *this );
    }

    virtual bool flush() = 0;

    virtual bool close() = 0;

private:
    template <typename Container>
    bool writeListLikeContainer( const Container& list ) {
        auto end = list.cend();
        for( auto itr = list.cbegin(); itr != end; ++itr ) {
            if( !write( SerializationMarker::Item ) ) {
                return false;
            }
            if( !write( *itr ) ) {
                return false;
            }
        }
        return write( SerializationMarker::End );
    }

    template <typename Container>
    bool writeMapLikeContainer( const Container& map ) {
        auto end = map.cend();
        for( auto itr = map.cbegin(); itr != end; ++itr ) {
            if( !write( SerializationMarker::Item ) ) {
                return false;
            }
            if( !write( itr->first ) || !write( itr->second ) ) {
                return false;
            }
        }
        return write( SerializationMarker::End );
    }
};

class ArrayWStream final : public IWriteStream {
    char* m_buf;
    std::size_t m_bufsize;
    std::size_t m_wpos = 0;

public:
    ArrayWStream( char* buf, std::size_t bufsize )
    : m_buf( buf )
    , m_bufsize( bufsize ) {}

    using IWriteStream::write;

    std::size_t write( const char* data, std::size_t dataSize ) override {
        std::size_t sizeToWrite = std::min( dataSize, freeSpace() );
        std::memcpy( m_buf + m_wpos, data, sizeToWrite );
        m_wpos += sizeToWrite;
        return sizeToWrite;
    }

    bool flush() override {
        m_wpos = 0;
        return true;
    }

    bool close() override {
        return flush();
    }

    inline std::size_t dataSize() const {
        return m_wpos;
    }

    inline std::size_t freeSpace() const {
        return m_bufsize - dataSize();
    }
};

}  // namespace common
