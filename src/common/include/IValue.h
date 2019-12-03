#pragma once

#include <memory>

namespace common {

class BoolValue;
class FloatValue;
class IntValue;
class NotificationValue;
class RgbValue;
class ScalebleValue;
class StringValue;
class UserCodeValue;
class ArrayValue;
class TableValue;

class IValueVisitorConst;
class IValueVisitor;
class IReadStream;

class IValue {
public:
    using UPtr = std::unique_ptr<IValue>;

    static UPtr create( IReadStream& stream );

    virtual ~IValue() = default;

    virtual void accept( IValueVisitorConst& visitor ) const = 0;

    virtual void accept( IValueVisitor& visitor ) = 0;

    virtual bool operator==( const IValue& value ) const = 0;

    virtual bool operator!=( const IValue& value ) const {
        return !( *this == value );
    }

    virtual UPtr clone() const = 0;
};

std::string name( const IValue& value );

bool operator>( const IValue& lval, const IValue& rval );

bool operator<( const IValue& lval, const IValue& rval );

bool operator>=( const IValue& lval, const IValue& rval );

bool operator<=( const IValue& lval, const IValue& rval );

}  // namespace common
