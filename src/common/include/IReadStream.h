#pragma once

#include <Buffer.h>
#include <StlBlockTypes.h>

#include <cinttypes>
#include <iterator>
#include <cstring>
#include <string>
#include <memory>
#include <list>
#include <vector>
#include <unordered_map>
#include <set>

namespace common {

class IReadStream {
    enum class SerializationMarker : std::uint8_t { Item, End };

public:
    using UPtr = std::unique_ptr<IReadStream>;

    virtual ~IReadStream() = default;

    virtual size_t read( char* data, size_t dataSize ) = 0;

    template <typename ValueType,
              typename std::enable_if<std::is_integral<ValueType>::value || std::is_floating_point<ValueType>::value ||
                                          std::is_enum<ValueType>::value,
                                      int>::type = 0>
    inline bool read( ValueType& value ) {
        if( sizeof( ValueType ) > size() ) {
            return false;
        }
        return sizeof( ValueType ) == read( reinterpret_cast<char*>( &value ), sizeof( ValueType ) );
    }

    bool read( std::string& value ) {
        std::size_t itemsCount;
        if( !read( itemsCount ) ) {
            return false;
        }
        value.reserve( itemsCount );
        auto beginItr = this->begin<std::string::traits_type::char_type>( itemsCount );
        auto endItr = this->end<std::string::traits_type::char_type>();
        value = std::move( std::string( beginItr, endItr ) );
        return value.size() == itemsCount;
    }

    template <typename T>
    bool read( std::list<T>& list ) {
        return readListLikeContainer( list );
    }

    template <typename T>
    bool read( block_list<T>& list ) {
        return readListLikeContainer( list );
    }

    template <typename ValueType>
    bool read( std::vector<ValueType>& vec ) {
        std::size_t itemsCount;
        if( !read( itemsCount ) ) {
            return false;
        }
        auto begin = this->begin<ValueType>( itemsCount );
        auto end = this->end<ValueType>();
        vec = std::move( std::vector<ValueType>( begin, end ) );
        return vec.size() == itemsCount;
    }

    bool read( Buffer& value ) {
        std::size_t strLen;
        if( !read( strLen ) ) {
            return false;
        }
        value = std::move( Buffer( *this, strLen ) );
        return bool( value );
    }

    template <typename T>
    bool read( std::unique_ptr<T>& uptr ) {
        auto instanceUPtr = T::create( *this );
        if( instanceUPtr ) {
            uptr = std::move( instanceUPtr );
            return true;
        }
        else {
            uptr = nullptr;
            return false;
        }
    }

    template <typename Key, typename Value>
    bool read( std::unordered_map<Key, Value>& map ) {
        SerializationMarker marker;
        while( read( marker ) ) {
            if( SerializationMarker::End == marker ) {
                return true;
            }
            else if( SerializationMarker::Item != marker ) {
                return false;
            }

            Key key;
            Value value;
            if( !read( key ) || !read( value ) ) {
                return false;
            }

            map.emplace( key, value );
        }
        return false;
    }

    template <typename Value>
    bool read( std::set<Value>& set ) {
        SerializationMarker marker;
        while( read( marker ) ) {
            if( SerializationMarker::End == marker ) {
                return true;
            }
            else if( SerializationMarker::Item != marker ) {
                return false;
            }

            Value val;
            if( !read( val ) ) {
                return false;
            }
            set.insert( std::move( val ) );
        }
        return false;
    }

    template <typename T, typename std::enable_if<std::is_class<T>::value, int>::type = 0>
    inline bool read( T& object ) {
        return object.read( *this );
    }

    virtual size_t size() const {
        return 0;
    }

    virtual bool reset(unsigned int offset) {
    	return false;
    }

    template <typename ValueType>
    class Iterator : public std::iterator<std::input_iterator_tag, ValueType> {
        static const int END_OF_ITERATIONS = -1;

        IReadStream& m_rstream;
        int m_itemsLeft;
        ValueType m_value;

    public:
        explicit Iterator( IReadStream& rstream, int itemsCount = END_OF_ITERATIONS )
        : m_rstream( rstream )
        , m_itemsLeft( itemsCount ) {
            ++( *this );
        }

        Iterator& operator++() {
            if( m_rstream.size() < sizeof( ValueType ) || m_itemsLeft <= 0 ) {
                m_itemsLeft = END_OF_ITERATIONS;
                return *this;
            }

            if( m_rstream.read( m_value ) ) {
                --m_itemsLeft;
            }
            else {
                // If we can't read from stream simply terminate iterations
                m_itemsLeft = END_OF_ITERATIONS;
            }
            return *this;
        }

        Iterator operator++( int ) {
            Iterator retval = *this;
            ++( *this );
            return retval;
        }

        bool operator==( const Iterator& other ) const {
            return m_itemsLeft == other.m_itemsLeft;
        }

        bool operator!=( const Iterator& other ) const {
            return !( *this == other );
        }

        const ValueType& operator*() const {
            return m_value;
        }
    };

    template <typename ValueType>
    Iterator<ValueType> begin( std::size_t itemsCount ) {
        if( size() < itemsCount ) {
            return end<ValueType>();
        }
        return Iterator<ValueType>( *this, itemsCount );
    }

    template <typename ValueType>
    Iterator<ValueType> end() {
        return Iterator<ValueType>( *this );
    }

private:
    template <typename Container>
    bool readListLikeContainer( Container& list ) {
        SerializationMarker marker;
        while( read( marker ) ) {
            if( SerializationMarker::End == marker ) {
                return true;
            }
            else if( SerializationMarker::Item != marker ) {
                return false;
            }

            auto itr = list.emplace( list.end() );
            if( !read( *itr ) ) {
                return false;
            }
        }
        return false;
    }
};

class ArrayRStream final : public IReadStream {
    char* m_buf;
    std::size_t m_bufsize;
    std::size_t m_rpos = 0;

public:
    ArrayRStream( char* buf, std::size_t bufsize )
    : m_buf( buf )
    , m_bufsize( bufsize ) {}

    using IReadStream::read;

    std::size_t read( char* data, std::size_t dataSize ) override {
        std::size_t sizeToRead = std::min( dataSize, size() );
        std::memcpy( data, m_buf + m_rpos, sizeToRead );
        m_rpos += sizeToRead;
        return sizeToRead;
    }

    inline std::size_t size() const override {
        return m_bufsize - m_rpos;
    }
};

}  // namespace common
