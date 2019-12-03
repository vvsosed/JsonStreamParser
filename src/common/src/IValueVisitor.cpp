#include "IValueVisitor.h"
#include "IWriteStream.h"

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

#include <cstring>

//#define ENABLE_IVV_LOGS
#ifdef ENABLE_IVV_LOGS

#include <DebugUtils.h>

#define IVV_LOG_TAG "ivvisivvisivvis [IValueVisitor] "
#define IVV_LOGI( _FMT, ... ) ESP_LOGI( IVV_LOG_TAG, _FMT, ##__VA_ARGS__ )
#define IVV_LOGE( _FMT, ... ) ESP_LOGE( IVV_LOG_TAG, _FMT, ##__VA_ARGS__ )
#define IVV_LOGW( _FMT, ... ) ESP_LOGW( IVV_LOG_TAG, _FMT, ##__VA_ARGS__ )

#else

#define IVV_LOGI( ... ) \
    do {                \
    } while( false )
#define IVV_LOGE( ... ) \
    do {                \
    } while( false )
#define IVV_LOGW( ... ) \
    do {                \
    } while( false )

#endif

namespace common {

bool SerializeVisitor::serialize( const IValue& value ) {
    value.accept( *this );
    return *this;
}

void SerializeVisitor::visit( const FloatValue& value ) {
    IVV_LOGI( "serialize FloatValue" );
    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( FloatVal ) ) && m_stream.write( value.get() );
}

void SerializeVisitor::visit( const ScalebleValue& value ) {
    IVV_LOGI( "serialize ScalebleValue" );
    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( ScalebleVal ) ) && m_stream.write( value.get() ) &&
                  m_stream.write( value.getType() );
    std::string scale = value.getScale();
    std::size_t scaleLen = scale.length();
    m_isSuccess =
        m_isSuccess && m_stream.write( scaleLen ) && ( scaleLen == m_stream.write( scale.c_str(), scaleLen ) );
}

void SerializeVisitor::visit( const UserCodeValue& value ) {
    IVV_LOGI( "serialize UserCodeValue" );
    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( UserCodeVal ) );
    m_isSuccess = m_isSuccess && m_stream.write( value.getName() );
    m_isSuccess = m_isSuccess && m_stream.write( value.getCode() );
}

void SerializeVisitor::visit( const IntValue& value ) {
    IVV_LOGI( "serialize IntValue" );
    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( IntVal ) ) && m_stream.write( value.get() );
}

void SerializeVisitor::visit( const BoolValue& value ) {
    IVV_LOGI( "serialize BoolValue" );
    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( BoolVal ) ) && m_stream.write( value.get() );
}

void SerializeVisitor::visit( const StringValue& value ) {
    IVV_LOGI( "serialize StringValue" );

    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( StringVal ) );
    std::size_t scaleLen = 0;

    if( value.get() ) {
        scaleLen = std::strlen( value.get() );
    }
    else {
        IVV_LOGW( "serialize StringValue.  Value is wrong" );
    }

    m_isSuccess = m_isSuccess && m_stream.write( scaleLen );
    if( scaleLen ) {
        m_isSuccess = m_isSuccess && ( scaleLen == m_stream.write( value.get(), scaleLen ) );
    }
}

void SerializeVisitor::visit( const RgbValue& value ) {
    IVV_LOGI( "serialize RgbValue" );
    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( RgbVal ) );
    const RgbValue::RgbData& data = value.get();
    std::size_t itemsCount = data.size();
    m_isSuccess = m_isSuccess && m_stream.write( itemsCount );
    for( int i = 0; i < itemsCount && m_isSuccess; ++i ) {
        const std::uint8_t color = std::uint8_t( data[i].color );
        m_isSuccess = m_isSuccess && m_stream.write( color );
        const std::uint8_t value = data[i].value;
        m_isSuccess = m_isSuccess && m_stream.write( value );
    }
}

void SerializeVisitor::visit( const NotificationValue& value ) {
    IVV_LOGI( "serialize NotificationValue" );
    m_isSuccess = m_isSuccess && m_stream.write( std::size_t( NotificationVal ) ) &&
                  m_stream.write( std::uint8_t( value.getAlarmType() ) ) &&
                  m_stream.write( std::uint8_t( value.getAlarmLevel() ) );
    std::size_t itemsCount = value.getData().size();
    std::size_t dataSize = itemsCount * sizeof( NotificationValue::BlobValueType );
    m_isSuccess = m_isSuccess && m_stream.write( itemsCount ) &&
                  ( dataSize == m_stream.write( reinterpret_cast<const char*>( value.getData().data() ), dataSize ) );
};

void SerializeVisitor::visit( const ArrayValue& value ) {
    IVV_LOGI( "serialize ArrayValue" );
    m_stream.write( std::size_t( ArrayVal ) );
    for( auto& val : value ) {
        val->accept( *this );
    }
    m_stream.write( std::size_t( EndOfArrayVal ) );
}

void SerializeVisitor::visit( const TableValue& value ) {
    IVV_LOGI( "serialize TableValue" );
    m_stream.write( std::size_t( TableVal ) );
    auto itr = value.cbegin();
    auto end = value.cend();
    for( ; itr != end; ++itr ) {
        auto key = itr->first;
        StringValue keyVal( key.c_str(), key.length() );
        keyVal.accept( *this );
        itr->second->accept( *this );
    }
    m_stream.write( std::size_t( EndOfTableVal ) );
}

}  // namespace common
