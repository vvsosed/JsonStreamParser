
#pragma once

#include <values/FloatValue.h>
#include <string>
#include <stdio.h>

namespace common {

class ScalebleValue final : public FloatValue {
public:
    ScalebleValue() = default;

    ScalebleValue( const ScalebleValue& ) = default;

    ScalebleValue( FloatType value );

    ScalebleValue( FloatType value, std::string&& scale, std::uint8_t type );

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    const std::string& getScale() const {
        return m_scale;
    }

    std::uint8_t getType() const {
        return m_type;
    }

    void setScale( std::string scale ) {
        m_scale = scale;
    }

    void setType( std::uint8_t type ) {
        m_type = type;
    }

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new ScalebleValue( *this ) );
    };

private:
    std::string m_scale;
    std::uint8_t m_type;
};

}  // namespace common
