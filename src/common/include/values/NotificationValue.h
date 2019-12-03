#pragma once

#include <IValue.h>
#include <stdint.h>
#include <vector>

namespace common {
// TODO: Change class to remove dependancy on ZNet library
class NotificationValue final : public IValue {
public:
    using BlobValueType = uint8_t;
    using BlobType = std::vector<BlobValueType>;

    NotificationValue() = default;

    NotificationValue( const NotificationValue& ) = default;

    NotificationValue( uint8_t v1AlarmType,
                       uint8_t v1AlarmLevel,
                       const uint8_t* data,
                       uint8_t dataSize );

    NotificationValue( uint8_t v1AlarmType,
                       uint8_t v1AlarmLevel,
                       BlobType&& data );

    const BlobType& getData() const {
        return m_data;
    }

    uint8_t getAlarmType() const {
        return m_v1AlarmType;
    }

    uint8_t getAlarmLevel() const {
        return m_v1AlarmLevel;
    }

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new NotificationValue( *this ) );
    };

private:
    uint8_t m_v1AlarmType;
    uint8_t m_v1AlarmLevel;
    BlobType m_data;
};

}  // namespace common
