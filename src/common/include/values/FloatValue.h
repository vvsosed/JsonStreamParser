
#pragma once

#include <IValue.h>

namespace common {

class FloatValue : public IValue {
public:
    using FloatType = float;

    FloatValue() = default;

    FloatValue( const FloatValue& ) = default;

    FloatValue( float value );

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    FloatType get() const {
        return m_value;
    }

    void set( FloatType value ) {
        m_value = value;
    }

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new FloatValue( *this ) );
    };

private:
    FloatType m_value = 0.0f;
    static constexpr FloatType m_epsilon = 0.001f;  //  0.1 %
};

}  // namespace common
