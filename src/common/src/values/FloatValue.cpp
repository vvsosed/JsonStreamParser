
#include <values/FloatValue.h>
#include <IValueCast.h>
#include <cmath>
#include <stdio.h>

namespace common {

FloatValue::FloatValue( float value )
: m_value( value ) {}

void FloatValue::accept( common::IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void FloatValue::accept( common::IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool FloatValue::operator==( const IValue& value ) const {
    auto floatVal = iValueCast<FloatValue>( &value );
    if( !floatVal ) {
        return false;
    }

    return std::fabs( floatVal->get() - get() ) < m_epsilon * get();
}

}  // namespace common
