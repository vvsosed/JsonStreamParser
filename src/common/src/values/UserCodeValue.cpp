
#include <values/UserCodeValue.h>
#include <IValueCast.h>

namespace common {

UserCodeValue::UserCodeValue( std::string&& name, std::string&& code )
: m_name( name )
, m_code( code ) {}

void UserCodeValue::accept( common::IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void UserCodeValue::accept( common::IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool UserCodeValue::operator==( const IValue& value ) const {
    auto userCodeVal = iValueCast<UserCodeValue>( &value );
    if( !userCodeVal ) {
        return false;
    }

    return userCodeVal->getName() == getName() && userCodeVal->getCode() == getCode();
}

}  // namespace common
