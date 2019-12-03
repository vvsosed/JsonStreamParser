
#pragma once

#include "IValue.h"
#include <stdint.h>

namespace common {

class BoolValue final : public IValue {
public:
    static constexpr uint8_t Low = 0;
    static constexpr uint8_t High = 255;

public:
    BoolValue() = default;

    BoolValue( const BoolValue& ) = default;

    BoolValue( bool value );

    bool get() const;

    void set( bool value );

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new BoolValue( *this ) );
    };

private:
    bool m_value = false;
};

}  // namespace common
