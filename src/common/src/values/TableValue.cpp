#include "../include/values/TableValue.h"
#include "../include/IValueCast.h"
#include <utility>

namespace common {

void TableValue::accept( IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void TableValue::accept( IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool TableValue::operator==( const IValue& value ) const {
    auto tblVal = iValueCast<TableValue>( &value );
    if( !tblVal ) {
        return false;
    }

    return m_value == tblVal->m_value;
}

void TableValue::insert( const Key& key, ValPtr value ) {
    m_value.insert( std::make_pair( key, std::move( value ) ) );
}

TableValue::UPtr TableValue::clone() const {
    auto cloned_val = std::make_unique<TableValue>();
    if( !cloned_val ) {
        return nullptr;
    }

    for( auto& child_val : m_value ) {
        auto cloned_child_val = child_val.second->clone();
        if( !cloned_child_val ) {
            return nullptr;
        }
        cloned_val->m_value.emplace( child_val.first, std::move( cloned_child_val ) );
    };

    return cloned_val;
};

}  // end of namespace common
