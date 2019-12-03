#include <values/NotificationValue.h>
#include <IValueCast.h>

namespace common {

NotificationValue::NotificationValue( uint8_t v1AlarmType,
                                      uint8_t v1AlarmLevel,
                                      const uint8_t* data,
                                      uint8_t dataSize )
: m_v1AlarmType( v1AlarmType )
, m_v1AlarmLevel( v1AlarmLevel )
, m_data( &data[0u], &data[dataSize] ) {}

NotificationValue::NotificationValue( uint8_t v1AlarmType,
                                      uint8_t v1AlarmLevel,
                                      BlobType&& data )
: m_v1AlarmType( v1AlarmType )
, m_v1AlarmLevel( v1AlarmLevel )
, m_data( std::move( data ) ) {}

void NotificationValue::accept( IValueVisitorConst& visitor ) const {
    visitor.visit( *this );
}

void NotificationValue::accept( IValueVisitor& visitor ) {
    visitor.visit( *this );
}

bool NotificationValue::operator==( const IValue& value ) const {
    auto notifVal = iValueCast<NotificationValue>( &value );
    if( !notifVal ) {
        return false;
    }

    return m_v1AlarmType == notifVal->m_v1AlarmType && m_v1AlarmLevel == notifVal->m_v1AlarmLevel;
}

}  // namespace common
