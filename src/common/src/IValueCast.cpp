#include "IValueCast.h"

#include "values/FloatValue.h"
#include "values/ScalebleValue.h"
#include "values/IntValue.h"
#include "values/TableValue.h"
#include "values/StringValue.h"
#include "values/FloatValue.h"
#include "values/UserCodeValue.h"

#include "DebugUtils.h"
#include "CommonResources.h"

#include <cmath>
#include <cstring>

namespace common {

enum class ItemValueType {
    NoValue = 0,

    Int,
    Bool,
    Float,
    RGB,
    Usercode,
    String,
    CameraStream,
    Token,
    Illuminance,
    Pressure,
    Substance_amount,
    Power,
    Humidity,
    Velocity,
    Acceleration,
    Direction,
    General_purpose,
    Acidity,
    Electric_potential,
    Electric_current,
    Force,
    Irradiance,
    Precipitation,
    Length,
    Mass,
    Volume_flow,
    Volume,
    Angle,
    Frequency,
    Seismic_intensity,
    Seismic_magnitude,
    Ultraviolet,
    Electrical_resistance,
    Electrical_conductivity,
    Loudness,
    Moisture,
    Time,
    Radon_concentration,
    Blood_pressure,
    Energy,
    Rf_signal_strength,
    Temperature,
    Dictionary,
    Button_state,

};

ItemValueType toItemValueType( const char* typeName ) {
    using namespace common::resources;

    FUNC_LOGD( "typeName: %s", typeName );

    if( strcmp( values::Int, typeName ) == 0 ) {
        return ItemValueType::Int;
    }
    else if( strcmp( values::Bool, typeName ) == 0 ) {
        return ItemValueType::Bool;
    }
    else if( strcmp( values::Float, typeName ) == 0 ) {
        return ItemValueType::Float;
    }
    else if( strcmp( values::RGB, typeName ) == 0 ) {
        return ItemValueType::RGB;
    }
    else if( strcmp( values::Usercode, typeName ) == 0 ) {
        return ItemValueType::Usercode;
    }
    else if( strcmp( values::String, typeName ) == 0 ) {
        return ItemValueType::String;
    }
    else if( strcmp( values::Token, typeName ) == 0 ) {
        return ItemValueType::Token;
    }
    else if( strcmp( values::CameraStream, typeName ) == 0 ) {
        return ItemValueType::CameraStream;
    }
    else if( strcmp( values::Illuminance, typeName ) == 0 ) {
        return ItemValueType::Illuminance;
    }
    else if( strcmp( values::Pressure, typeName ) == 0 ) {
        return ItemValueType::Pressure;
    }
    else if( strcmp( values::Substance_amount, typeName ) == 0 ) {
        return ItemValueType::Substance_amount;
    }
    else if( strcmp( values::Power, typeName ) == 0 ) {
        return ItemValueType::Power;
    }
    else if( strcmp( values::Humidity, typeName ) == 0 ) {
        return ItemValueType::Humidity;
    }
    else if( strcmp( values::Velocity, typeName ) == 0 ) {
        return ItemValueType::Velocity;
    }
    else if( strcmp( values::Acceleration, typeName ) == 0 ) {
        return ItemValueType::Acceleration;
    }
    else if( strcmp( values::Direction, typeName ) == 0 ) {
        return ItemValueType::Direction;
    }
    else if( strcmp( values::General_purpose, typeName ) == 0 ) {
        return ItemValueType::General_purpose;
    }
    else if( strcmp( values::Acidity, typeName ) == 0 ) {
        return ItemValueType::Acidity;
    }
    else if( strcmp( values::Electric_potential, typeName ) == 0 ) {
        return ItemValueType::Electric_potential;
    }
    else if( strcmp( values::Electric_current, typeName ) == 0 ) {
        return ItemValueType::Electric_current;
    }
    else if( strcmp( values::Force, typeName ) == 0 ) {
        return ItemValueType::Force;
    }
    else if( strcmp( values::Irradiance, typeName ) == 0 ) {
        return ItemValueType::Irradiance;
    }
    else if( strcmp( values::Precipitation, typeName ) == 0 ) {
        return ItemValueType::Precipitation;
    }
    else if( strcmp( values::Length, typeName ) == 0 ) {
        return ItemValueType::Length;
    }
    else if( strcmp( values::Mass, typeName ) == 0 ) {
        return ItemValueType::Mass;
    }
    else if( strcmp( values::Volume_flow, typeName ) == 0 ) {
        return ItemValueType::Volume_flow;
    }
    else if( strcmp( values::Volume, typeName ) == 0 ) {
        return ItemValueType::Volume;
    }
    else if( strcmp( values::Angle, typeName ) == 0 ) {
        return ItemValueType::Angle;
    }
    else if( strcmp( values::Frequency, typeName ) == 0 ) {
        return ItemValueType::Frequency;
    }
    else if( strcmp( values::Seismic_intensity, typeName ) == 0 ) {
        return ItemValueType::Seismic_intensity;
    }
    else if( strcmp( values::Seismic_magnitude, typeName ) == 0 ) {
        return ItemValueType::Seismic_magnitude;
    }
    else if( strcmp( values::Ultraviolet, typeName ) == 0 ) {
        return ItemValueType::Ultraviolet;
    }
    else if( strcmp( values::Electrical_resistance, typeName ) == 0 ) {
        return ItemValueType::Electrical_resistance;
    }
    else if( strcmp( values::Electrical_conductivity, typeName ) == 0 ) {
        return ItemValueType::Electrical_conductivity;
    }
    else if( strcmp( values::Loudness, typeName ) == 0 ) {
        return ItemValueType::Loudness;
    }
    else if( strcmp( values::Moisture, typeName ) == 0 ) {
        return ItemValueType::Moisture;
    }
    else if( strcmp( values::Time, typeName ) == 0 ) {
        return ItemValueType::Time;
    }
    else if( strcmp( values::Radon_concentration, typeName ) == 0 ) {
        return ItemValueType::Radon_concentration;
    }
    else if( strcmp( values::Blood_pressure, typeName ) == 0 ) {
        return ItemValueType::Blood_pressure;
    }
    else if( strcmp( values::Energy, typeName ) == 0 ) {
        return ItemValueType::Energy;
    }
    else if( strcmp( values::Rf_signal_strength, typeName ) == 0 ) {
        return ItemValueType::Rf_signal_strength;
    }
    else if( strcmp( values::Temperature, typeName ) == 0 ) {
        return ItemValueType::Temperature;
    }
    else if( strcmp( values::Button_state, typeName ) == 0 ) {
        return ItemValueType::Button_state;
    }
    else if( strcmp( values::Dictionary, typeName ) == 0 ) {
        return ItemValueType::Dictionary;
    }

    return ItemValueType::NoValue;
}

template <>
float iValueCast<float>( const IValue& value ) {
    class FloatVisitor : public IValueVisitorConst {
        float m_value = NAN;

    public:
        void visit( const FloatValue& value ) override {
            m_value = value.get();
        };

        void visit( const ScalebleValue& value ) override {
            m_value = value.get();
        };

        void visit( const IntValue& value ) override {
            m_value = value.get();
        };

        float value( const IValue& value ) {
            value.accept( *this );
            return m_value;
        }
    };

    return FloatVisitor().value( value );
}

template <>
std::unique_ptr<ScalebleValue> iValueCast<std::unique_ptr<ScalebleValue> >( const IValue::UPtr& value ) {
    class Visitor : public IValueVisitorConst {
        std::unique_ptr<ScalebleValue> m_value;

    public:
        void visit( const ScalebleValue& value ) override {
            m_value = std::unique_ptr<ScalebleValue>( new ScalebleValue( value ) );
        };

        void visit( const TableValue& value ) override {
            auto scale = iValueCast<StringValue>( value["scale"] );
            if( !scale ) {
                return;
            }

            auto typeStr = iValueCast<StringValue>( value["type"] );
            if( !typeStr ) {
                return;
            }
            auto typeId = toItemValueType( typeStr->get() );
            if( ItemValueType::NoValue >= typeId || ItemValueType::NoValue <= typeId ) {
                return;
            }

            auto val = iValueCast<FloatValue>( value["value"] );
            if( !scale ) {
                return;
            }

            m_value = std::make_unique<ScalebleValue>( val->get(), scale->get(), std::uint8_t( typeId ) );
        };

        std::unique_ptr<ScalebleValue> convert( const IValue& value ) {
            value.accept( *this );
            return std::move( m_value );
        }
    };

    return Visitor().convert( *value );
}

template <>
std::unique_ptr<UserCodeValue> iValueCast<std::unique_ptr<UserCodeValue> >( const IValue::UPtr& value ) {
    class Visitor : public IValueVisitorConst {
        std::unique_ptr<UserCodeValue> m_value;

    public:
        void visit( const UserCodeValue& value ) override {
            m_value = std::unique_ptr<UserCodeValue>( new UserCodeValue( value ) );
        };

        void visit( const TableValue& value ) override {
            auto code = iValueCast<StringValue>( value["code"] );
            if( !code ) {
                return;
            }

            auto name = iValueCast<StringValue>( value["name"] );
            if( !name ) {
                return;
            }

            m_value = std::make_unique<UserCodeValue>( name->get(), code->get() );
        };

        std::unique_ptr<UserCodeValue> convert( const IValue& value ) {
            value.accept( *this );
            return std::move( m_value );
        }
    };

    return Visitor().convert( *value );
}

}  // end of namespace common
