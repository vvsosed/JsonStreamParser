#pragma once

#include "IValue.h"
#include <list>
#include <memory>

namespace common {

class ArrayValue final : public IValue {
public:
    using ValPtr = IValue::UPtr;
    using ValList = std::list<ValPtr>;
    using Itr = ValList::iterator;
    using ConstItr = ValList::const_iterator;

public:
    ArrayValue() = default;

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override;

    void push_back( ValPtr value );

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

private:
    ValList m_value;
};

}  // namespace common
