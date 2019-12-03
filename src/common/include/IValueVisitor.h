#pragma once

#include <cstdint>

namespace common {

class IValue;
class FloatValue;
class ScalebleValue;
class IntValue;
class BoolValue;
class StringValue;
class RgbValue;
class NotificationValue;
class UserCodeValue;
class ArrayValue;
class TableValue;

class IWriteStream;

class IValueVisitorConst {
public:
    virtual ~IValueVisitorConst() = default;

public:
    virtual void visit( const FloatValue& value ){};

    virtual void visit( const ScalebleValue& value ){};

    virtual void visit( const IntValue& value ){};

    virtual void visit( const BoolValue& value ){};

    virtual void visit( const StringValue& value ){};

    virtual void visit( const RgbValue& value ){};

    virtual void visit( const NotificationValue& value ){};

    virtual void visit( const UserCodeValue& value ){};

    virtual void visit( const ArrayValue& value ){};

    virtual void visit( const TableValue& value ){};
};

class IValueVisitor {
public:
    virtual ~IValueVisitor() = default;

public:
    virtual void visit( FloatValue& value ){};

    virtual void visit( ScalebleValue& value ){};

    virtual void visit( IntValue& value ){};

    virtual void visit( BoolValue& value ){};

    virtual void visit( StringValue& value ){};

    virtual void visit( RgbValue& value ){};

    virtual void visit( NotificationValue& value ){};

    virtual void visit( UserCodeValue& value ){};

    virtual void visit( ArrayValue& value ){};

    virtual void visit( TableValue& value ){};
};

enum IValueId : std::size_t {
    FloatVal,
    ScalebleVal,
    IntVal,
    BoolVal,
    StringVal,
    RgbVal,
    NotificationVal,
    ArrayVal,
    EndOfArrayVal,
    TableVal,
    EndOfTableVal,
    UserCodeVal,
};

class SerializeVisitor : public IValueVisitorConst {
    IWriteStream& m_stream;
    bool m_isSuccess = true;

public:
    SerializeVisitor( IWriteStream& stream )
    : m_stream( stream ){};

    operator bool() const {
        return m_isSuccess;
    }

    bool serialize( const IValue& value );

    void visit( const FloatValue& value ) override;

    void visit( const ScalebleValue& value ) override;

    void visit( const IntValue& value ) override;

    void visit( const BoolValue& value ) override;

    void visit( const StringValue& value ) override;

    void visit( const RgbValue& value ) override;

    void visit( const NotificationValue& value ) override;

    void visit( const UserCodeValue& value ) override;

    void visit( const ArrayValue& value ) override;

    void visit( const TableValue& value ) override;
};

}  // namespace common
