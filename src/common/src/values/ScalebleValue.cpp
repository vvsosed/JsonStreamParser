
#include <values/ScalebleValue.h>
#include <IValueCast.h>

namespace common {

ScalebleValue::ScalebleValue( FloatType value )
: FloatValue( value ) {}

ScalebleValue::ScalebleValue( FloatType value, std::string&& scale, std::uint8_t type )
: FloatValue( value )
, m_scale( scale )
, m_type( type ) {}

void ScalebleValue::accept( common::IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void ScalebleValue::accept( common::IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool ScalebleValue::operator==( const IValue& value ) const {
    auto scalVal = iValueCast<ScalebleValue>( &value );
    if( !scalVal ) {
        return false;
    }

    return scalVal->get() == get() && scalVal->getScale() == getScale() && scalVal->getType() == getType();
}

}  // namespace common
