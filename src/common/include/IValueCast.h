#pragma once

#include "IValue.h"
#include "IValueVisitor.h"
#include "values/ScalebleValue.h"

#include <memory>
#include <cmath>

namespace common {

template <typename T>
std::unique_ptr<T> iValueCast( IValue::UPtr&& value ) {
    if( !value ) {
        return nullptr;
    }

    struct Visitor : IValueVisitorConst {
        const T* res = nullptr;

        void visit( const T& value ) override {
            res = &value;
        };
    } visitor;

    value->accept( visitor );

    return visitor.res ? std::unique_ptr<T>( static_cast<T*>( value.release() ) ) : nullptr;
}

template <typename T>
T* iValueCast( IValue* value ) {
    if( !value ) {
        return nullptr;
    }

    struct Visitor : IValueVisitor {
        T* res = nullptr;

        void visit( T& value ) override {
            res = &value;
        };
    } visitor;

    value->accept( visitor );

    return visitor.res;
}

template <typename T>
const T* iValueCast( const IValue* value ) {
    return iValueCast<T>( const_cast<IValue*>( value ) );
}

// Conversion to POD types

template <typename T>
T iValueCast( const IValue& value );

template <typename T>
T iValueCast( const IValue::UPtr& value );

template <>
float iValueCast<float>( const IValue& value );

template <>
inline float iValueCast<float>( const IValue::UPtr& value ) {
    return value ? iValueCast<float>( *value ) : NAN;
}

// Conversion from Table

template <>
std::unique_ptr<ScalebleValue> iValueCast<std::unique_ptr<ScalebleValue>>( const IValue::UPtr& value );

template <>
std::unique_ptr<UserCodeValue> iValueCast<std::unique_ptr<UserCodeValue>>( const IValue::UPtr& value );

}  // end of namespace common
