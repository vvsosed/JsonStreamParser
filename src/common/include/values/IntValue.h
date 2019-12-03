
#pragma once

#include <IValue.h>
#include <cstdint>

namespace common {

class IntValue final : public IValue {
public:
    using IntType = int32_t;

    IntValue() = default;

    IntValue( const IntValue& ) = default;

    IntValue( IntType value );

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    IntType get() const {
        return m_value;
    }

    void set( IntType value ) {
        m_value = value;
    }

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new IntValue( *this ) );
    };

private:
    IntType m_value = 0;
};

}  // namespace common
