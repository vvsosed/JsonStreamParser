
#pragma once

#include "StlBlockTypes.h"

#include <cstddef>
#include <cinttypes>
#include <string>
#include <list>
#include <set>

namespace common {

class IWriteStream;
class IReadStream;
class Buffer;
class IValue;

class JsonStreamGenerator final {
    enum class State { Root, ValuePart, FirstItem, Item };

public:
    JsonStreamGenerator( IWriteStream& outputStream );

    ~JsonStreamGenerator();

public:
    bool writePropertyKey( const char* key, std::uint16_t keySize );

    bool writePropertyKey( const char* key );

    bool writePropertyValue( const char* value, std::uint16_t valueSize );

    bool writePropertyValue( const char* value );

    bool writePropertyValue( const std::string& value ) {
        return writePropertyValue( value.c_str(), value.length() );
    }

    bool writeProperty( const char* key, std::uint16_t keySize, const char* value, std::uint16_t valueSize );

    bool writeProperty( const char* key, const char* value, std::uint16_t valueSize );

    bool writeProperty( const char* key, const char* value );

    bool writeProperty( const char* key, const std::string& value ) {
        return writeProperty( key, value.c_str(), value.size() );
    }

    bool writeProperty( const char* key, std::uint16_t keySize, float value );

    bool writeProperty( const char* key, std::uint16_t keySize, std::uint16_t value );

    bool writeProperty( const char* key, std::uint16_t keySize, std::uint32_t value );

    bool writeProperty( const char* key, std::uint16_t keySize, std::int16_t value );

    bool writeProperty( const char* key, std::uint16_t keySize, std::int32_t value );

    bool writeProperty( const char* key, std::uint16_t keySize, std::uint64_t value );

    bool writeProperty( const char* key, std::uint16_t keySize, std::int64_t value );

    bool writeProperty( const char* key, float value );

    bool writeProperty( const char* key, std::uint16_t value );

    bool writeProperty( const char* key, std::uint32_t value );

    bool writeProperty( const char* key, std::int16_t value );

    bool writeProperty( const char* key, std::int32_t value );

    bool writeProperty( const char* key, std::uint64_t value );

    bool writeProperty( const char* key, std::int64_t value );

    bool writeProperty( const char* key, std::uint16_t keySize, bool value );

    bool writeProperty( const char* key, bool value );

    bool writeProperty( const char* key, const Buffer& buf );

    bool writeProperty( const char* key, const IValue& value );

    bool writeProperty( const char* key, const IValue* value );

    bool writeProperty( const char* key, const std::nullptr_t ) {
        return writePropertyKey( key ) && writeNullValue() && writeItemDelimiter();
    }

    template <typename Type>
    bool writeProperty( const char* key, const std::list<Type>& list ) {
        return writeListLikeContainer( key, list );
    }

    template <typename Type>
    bool writeProperty( const char* key, const common::block_list<Type>& list ) {
        return writeListLikeContainer( key, list );
    }

    template <typename Type>
    bool writeProperty( const char* key, const std::set<Type>& set ) {
        return writeListLikeContainer( key, set );
    }

    void setFirstItemState() {
        m_state = State::FirstItem;
    }

    void setDept( int dept ) {
        m_depth = dept;
    }

    bool writeRawProperty( const char* key, const char* value );

    bool writeRawProperty( const char* key, IReadStream& stream );

    bool writeOpeningObject();

    bool writeOpeningArray();

    bool writeClosingSymbol();

    bool writeJsonEnd();

    bool writePropertyDelimiter();

    bool writeItemDelimiter();

    bool writeNullValue();

    bool writeEmptyObject();

private:
    template <typename Container>
    bool writeListLikeContainer( const char* key, const Container& container ) {
        if( !writePropertyKey( key ) || !writeOpeningArray() ) {
            return false;
        }

        for( const auto& value : container ) {
            if( !writeProperty( nullptr, value ) ) {
                return false;
            }
        }

        return writeClosingSymbol();
    }

    bool write( char symbol );

    bool write( const char* property, std::uint16_t size );

    bool write( const char* property );

    bool writeStringProperty( const char* property, std::uint16_t size );

    bool checkAndWriteOpeningBrackets();

    bool checkAndWriteComma();

private:
    bool incrementDepth();

    bool decrementDepth();

private:
    constexpr static int8_t MaxDepthSize = 32;

private:
    IWriteStream& m_outputStream;

    State m_state = State::Root;

    std::int8_t m_depth = 0;
    std::int32_t m_bracketsStack = 0;
    bool m_isOk = true;
};

}  // namespace common
