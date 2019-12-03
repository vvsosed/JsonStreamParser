#pragma once

#include "IValue.h"
#include <unordered_map>
#include <memory>
#include <string>

namespace common {

class TableValue final : public IValue {
public:
    using ValPtr = IValue::UPtr;
    using Key = std::string;
    using ValMap = std::unordered_map<Key, ValPtr>;
    using Itr = ValMap::iterator;
    using ConstItr = ValMap::const_iterator;

public:
    TableValue() = default;

    void accept( IValueVisitorConst& visitor ) const override;
    void accept( IValueVisitor& visitor ) override;

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override;

    void insert( const Key& key, ValPtr value );

    Itr begin() {
        return m_value.begin();
    }

    Itr end() {
        return m_value.end();
    }

    ConstItr begin() const {
        return m_value.cbegin();
    }

    ConstItr end() const {
        return m_value.cend();
    }

    ConstItr cbegin() const {
        return m_value.cbegin();
    }

    ConstItr cend() const {
        return m_value.cend();
    }

    bool isKeyExist( const Key& key ) const {
        return m_value.end() != m_value.find( key );
    }

    const IValue* operator[]( const Key& key ) const {
        auto itr = m_value.find( key );
        return m_value.end() == itr ? nullptr : itr->second.get();
    }

private:
    ValMap m_value;
};

}  // namespace common
