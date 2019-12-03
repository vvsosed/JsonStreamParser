
#include <values/BoolValue.h>
#include <IValueCast.h>

namespace common {

BoolValue::BoolValue( bool value )
: m_value( value ) {}

bool BoolValue::get() const {
    return m_value;
}

void BoolValue::set( bool value ) {
    m_value = value;
}

void BoolValue::accept( common::IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void BoolValue::accept( common::IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool BoolValue::operator==( const IValue& value ) const {
    auto boolVal = iValueCast<BoolValue>( &value );
    if( !boolVal ) {
        return false;
    }

    return boolVal->get() == get();
}

}  // namespace common
