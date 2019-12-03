#pragma once

#include <vector>
#include <IValue.h>

namespace common {

class RgbValue final : public IValue {
public:
    enum class Color : uint8_t {
        Unknown,

        Wwhite,
        Cwhite,
        Red,
        Green,
        Blue,
        Amber,
        Cyan,
        Purple,

        Indexed
    };

    struct ColorComponent {
        Color color;
        uint8_t value;

        ColorComponent( Color _color, uint8_t _value )
        : color( _color )
        , value( _value ) {}

        bool operator==( const ColorComponent& component ) const {
            return color == component.color && value == component.value;
        }
    };

    using RgbData = std::vector<ColorComponent>;
    using Iterator = RgbData::iterator;
    using ConstIterator = RgbData::const_iterator;

public:
    static const char* componentIdToStr( RgbValue::Color color );
    static RgbValue::Color strToColorComponentId( const char* str );

public:
    RgbValue();

    RgbValue( RgbData&& components );

    RgbValue( const RgbValue& ) = default;

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    size_t getComponentsCount() const;

    const RgbData& get() const;

    RgbData& get();

    Iterator findComponent( Color color );
    ConstIterator findComponent( Color color ) const;
    bool hasComponent( Color color ) const;
    uint8_t getComponentValue( Color color ) const;

    void set( RgbData&& components );
    void setComponentValue( Color color, uint8_t value );

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new RgbValue( *this ) );
    };

private:
    RgbData m_components;
};

}  // namespace common
