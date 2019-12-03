
#include <values/IntValue.h>

#include <IValueCast.h>

namespace common {
IntValue::IntValue( IntType value )
: m_value( value ) {}

void IntValue::accept( common::IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void IntValue::accept( common::IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool IntValue::operator==( const IValue& value ) const {
    auto intVal = iValueCast<IntValue>( &value );
    if( !intVal ) {
        return false;
    }

    return intVal->get() == get();
}

}  // namespace common
