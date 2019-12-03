
#pragma once

#include "IValue.h"

#include <Buffer.h>

#include <cinttypes>

namespace common {

class StringValue final : public IValue {
public:
    StringValue() = default;

    StringValue( const char* value, uint16_t size );

    StringValue( const char* value );

    StringValue( common::Buffer&& value );

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    const char* get() const;

    void set( const char* value, uint16_t size );

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new StringValue( m_value.get<char>() ) );
    };

private:
    common::Buffer m_value;
};

}  // namespace common
