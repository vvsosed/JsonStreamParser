#include <values/StringValue.h>
#include <IValueCast.h>
#include <string.h>

namespace common {

StringValue::StringValue( const char* value, uint16_t size ) {
    if( m_value.realloc( size + 1 ) ) {
        memcpy( m_value.get<char>(), value, size );
        m_value.get<char>()[size] = '\0';
    }
}

StringValue::StringValue( const char* value )
: m_value( value ) {}

StringValue::StringValue( common::Buffer&& value )
: m_value( std::move( value ) ){};

void StringValue::set( const char* value, uint16_t size ) {
    if( m_value.realloc( size + 1 ) ) {
        memcpy( m_value.get<char>(), value, size );
        m_value.get<char>()[size] = '\0';
    }
}

const char* StringValue::get() const {
    return m_value.get<char>();
}

void StringValue::accept( common::IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void StringValue::accept( common::IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool StringValue::operator==( const IValue& value ) const {
    if( !m_value ) {
        return false;
    }
    auto strVal = iValueCast<StringValue>( &value );
    if( !strVal ) {
        return false;
    }

    return 0 == strcmp( m_value.get<char>(), strVal->m_value.get<char>() );
}

}  // namespace common
