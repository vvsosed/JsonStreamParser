#include "../include/values/ArrayValue.h"
#include "../include/IValueCast.h"
#include <algorithm>

namespace common {

void ArrayValue::accept( IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void ArrayValue::accept( IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool ArrayValue::operator==( const IValue& value ) const {
    auto arrVal = iValueCast<ArrayValue>( &value );
    if( !arrVal ) {
        return false;
    }

    return m_value == arrVal->m_value;
}

void ArrayValue::push_back( ValPtr value ) {
    m_value.push_back( std::move( value ) );
}

ArrayValue::UPtr ArrayValue::clone() const {
    auto cloned_val = std::make_unique<ArrayValue>();
    if( !cloned_val ) {
        return nullptr;
    }

    for( auto& child_val : m_value ) {
        auto cloned_child_val = child_val->clone();
        if( !cloned_child_val ) {
            return nullptr;
        }
        cloned_val->m_value.emplace_back( std::move( cloned_child_val ) );
    };

    return cloned_val;
};

}  // end of namespace common
