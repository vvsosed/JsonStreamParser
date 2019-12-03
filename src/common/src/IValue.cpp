#include "IValue.h"
#include "IReadStream.h"
#include "IValueVisitor.h"
#include "IValueCast.h"

#include <values/FloatValue.h>
#include <values/ScalebleValue.h>
#include <values/IntValue.h>
#include <values/BoolValue.h>
#include <values/StringValue.h>
#include <values/RgbValue.h>
#include <values/NotificationValue.h>
#include <values/ArrayValue.h>
#include <values/TableValue.h>
#include <values/UserCodeValue.h>

#include "CommonResources.h"

//#define ENABLE_IVL_LOGS
#ifdef ENABLE_IVL_LOGS

#include <DebugUtils.h>

#define IVL_LOG_TAG "[IValue.cpp] "
#define IVL_LOGI( _FMT, ... ) ESP_LOGI( IVL_LOG_TAG, _FMT, ##__VA_ARGS__ )
#define IVL_LOGE( _FMT, ... ) ESP_LOGE( IVL_LOG_TAG, _FMT, ##__VA_ARGS__ )
#define IVL_LOGW( _FMT, ... ) ESP_LOGW( IVL_LOG_TAG, _FMT, ##__VA_ARGS__ )

#else

#define IVL_LOGI( ... ) \
    do {                \
    } while( false )
#define IVL_LOGE( ... ) \
    do {                \
    } while( false )
#define IVL_LOGW( ... ) \
    do {                \
    } while( false )

#endif

namespace common {

IValue::UPtr IValue::create( IReadStream& stream ) {
    std::size_t id;
    if( !stream.read( id ) ) {
        return nullptr;
    }

    switch( id ) {
    case FloatVal: {
        IVL_LOGI( "deserialize FloatValue" );
        FloatValue::FloatType value;
        return stream.read( value ) ? std::make_unique<FloatValue>( value ) : nullptr;
    }
    case ScalebleVal: {
        IVL_LOGI( "deserialize ScalebleValue" );
        ScalebleValue::FloatType value;
        std::uint8_t type;
        std::string scale;
        if( !stream.read( value ) || !stream.read( type ) || !stream.read( scale ) ) {
            return nullptr;
        }
        return std::make_unique<ScalebleValue>( value, std::move( scale ), type );
    }
    case IntVal: {
        IVL_LOGI( "deserialize IntValue" );
        IntValue::IntType value;
        return stream.read( value ) ? std::make_unique<IntValue>( value ) : nullptr;
    }
    case BoolVal: {
        IVL_LOGI( "deserialize BoolValue" );
        bool value;
        return stream.read( value ) ? std::make_unique<BoolValue>( value ) : nullptr;
    }
    case StringVal: {
        IVL_LOGI( "deserialize StringValue" );
        Buffer str;
        return stream.read( str ) ? std::make_unique<StringValue>( std::move( str ) ) : nullptr;
    }
    case RgbVal: {
        IVL_LOGI( "deserialize RgbValue" );
        std::size_t itemsCount;
        if( !stream.read( itemsCount ) ) {
            return nullptr;
        }
        RgbValue::RgbData data;
        data.reserve( itemsCount );
        for( int i = 0; i < itemsCount; ++i ) {
            std::uint8_t color, value;
            if( !stream.read( color ) || !stream.read( value ) ) {
                return nullptr;
            }
            data.emplace_back( RgbValue::Color( color ), value );
        }
        return std::make_unique<RgbValue>( std::move( data ) );
    }
    case NotificationVal: {
        IVL_LOGI( "deserialize NotificationValue" );

        std::uint8_t notification, event, alarmType, alarmLevel;
        NotificationValue::BlobType data;
        if( !stream.read( notification ) || !stream.read( event ) || !stream.read( alarmType ) ||
            !stream.read( alarmLevel ) || !stream.read( data ) ) {
            return nullptr;
        }
        return std::make_unique<NotificationValue>( alarmType,
                                                    alarmLevel,
                                                    std::move( data ) );
    }
    case ArrayVal: {
        IVL_LOGI( "deserialize ArrayValue" );
        auto arrayValue = std::make_unique<ArrayValue>();
        while( true ) {
            auto val = create( stream );
            if( !val ) {
                break;
            }
            arrayValue->push_back( std::move( val ) );
        }
        return arrayValue;
    }

    case TableVal: {
        IVL_LOGI( "deserialize TableValue" );
        auto tableValue = std::make_unique<TableValue>();
        while( true ) {
            auto key = iValueCast<StringValue>( create( stream ) );
            if( !key ) {
                return tableValue;
            }
            auto val = create( stream );
            if( !val ) {
                return nullptr;
            }

            tableValue->insert( key->get(), std::move( val ) );
        }
        return tableValue;
    }
    case UserCodeVal: {
        IVL_LOGI( "deserialize UserCodeValue" );
        std::string name;
        std::string code;
        if( !stream.read( name ) || !stream.read( code ) ) {
            return nullptr;
        }
        return std::make_unique<UserCodeValue>( std::move( name ), std::move( code ) );
    }
    case EndOfArrayVal:
    case EndOfTableVal:
    default:
        return nullptr;
    }
}

std::string name( const IValue& value ) {
    using namespace common::resources;

    struct NameVisitor : public IValueVisitorConst {
        std::string& m_name;

        NameVisitor( std::string& name )
        : m_name( name ){};

        void visit( const FloatValue& value ) override {
            m_name = values::Float;
        }

        void visit( const ScalebleValue& value ) override {
            m_name = value.getScale();
        }

        void visit( const IntValue& value ) override {
            m_name = values::Int;
        }

        void visit( const BoolValue& value ) override {
            m_name = values::Bool;
        }

        void visit( const StringValue& value ) override {
            m_name = values::String;
        }

        void visit( const RgbValue& value ) override {
            m_name = values::RGB;
        }

        void visit( const NotificationValue& value ) override {
            m_name = values::RGB;
        }

        void visit( const ArrayValue& value ) override {
            // #TODO not implemented yet
        }

        void visit( const TableValue& value ) override {
            // #TODO not implemented yet
        }

        void visit( const UserCodeValue& value ) override {
            m_name = values::UserCode;
        }
    };

    std::string name;
    NameVisitor visitor( name );
    value.accept( visitor );
    return std::move( name );
}

bool operator>( const IValue& lval, const IValue& rval ) {
    {
        auto leftFloat = iValueCast<FloatValue>( &lval );
        auto rightFloat = iValueCast<FloatValue>( &rval );
        if( leftFloat && rightFloat ) {
            return leftFloat->get() > rightFloat->get();
        }
    }

    {
        auto leftInt = iValueCast<IntValue>( &lval );
        auto rightInt = iValueCast<IntValue>( &rval );
        if( leftInt && rightInt ) {
            return leftInt->get() > rightInt->get();
        }
    }

    return false;
}

bool operator<( const IValue& lval, const IValue& rval ) {
    {
        auto leftFloat = iValueCast<FloatValue>( &lval );
        auto rightFloat = iValueCast<FloatValue>( &rval );
        if( leftFloat && rightFloat ) {
            return leftFloat->get() < rightFloat->get();
        }
    }

    {
        auto leftInt = iValueCast<IntValue>( &lval );
        auto rightInt = iValueCast<IntValue>( &rval );
        if( leftInt && rightInt ) {
            return leftInt->get() < rightInt->get();
        }
    }

    return false;
}

bool operator>=( const IValue& lval, const IValue& rval ) {
    return lval == rval || lval > rval;
}

bool operator<=( const IValue& lval, const IValue& rval ) {
    return lval == rval || lval < rval;
}

}  // namespace common
