#pragma once

namespace common {
namespace initialize_manager {

class IInitialize {
public:
    IInitialize()
    : m_isInitialized( false ) {}
    virtual ~IInitialize() = default;
    virtual bool isInitialized() {
        return m_isInitialized;
    }
    void setInitialized( bool value ) {
        m_isInitialized = value;
    }

protected:
    bool m_isInitialized;
};

}  // namespace initialize_manager

}  // namespace common
