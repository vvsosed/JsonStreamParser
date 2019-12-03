
#pragma once

#include <IValue.h>
#include <string>
#include <stdio.h>

namespace common {

class UserCodeValue final : public IValue {
public:
    UserCodeValue() = default;

    UserCodeValue( const UserCodeValue& ) = default;

    UserCodeValue( std::string&& name, std::string&& code );

    void accept( IValueVisitorConst& visitor ) const override;

    void accept( IValueVisitor& visitor ) override;

    const std::string& getName() const {
        return m_name;
    }

    const std::string& getCode() const {
        return m_code;
    }

    void setName( std::string name ) {
        m_name = name;
    }

    void setCode( std::string code ) {
        m_code = code;
    }

    bool operator==( const IValue& value ) const override;

    UPtr clone() const override {
        return UPtr( new UserCodeValue( *this ) );
    };

private:
    std::string m_name;
    std::string m_code;
};

}  // namespace common
