#pragma once

#include <cinttypes>
#include <memory>
#include <stack>

namespace common {

class IValue;
class IReadStream;

class JSStreamParser {
    struct Flags {
        enum ParseMode {
            Empty = 0,
            NotQuotedToken = 1 << 0,
            Value = 1 << 1,
            QuotedToken = 1 << 2,
            FloatToken = 1 << 3,
            TokenTruncated = 1 << 4
        };
    };

public:
    using UPtr = std::unique_ptr<JSStreamParser>;

    JSStreamParser( char* buffer, std::size_t size );

    virtual ~JSStreamParser() = default;

    bool parse( IReadStream& stream );

    inline void ignoreAllNestedBlocks() {
        m_ignoreLevel = m_depth;
    }

    inline bool isQuotedTokenProcessing() const {
        return m_stateFlags & Flags::QuotedToken;
    }

    inline bool isValueProcessing() const {
        return m_stateFlags & Flags::Value;
    }

    inline bool isNotQuotedTokenProcessing() const {
        return m_stateFlags & Flags::NotQuotedToken;
    }

    inline bool isFloatTokenProcessing() const {
        return m_stateFlags & Flags::FloatToken;
    }

    inline const char* getTokenValue() const {
        return m_buffer;
    }

    inline std::size_t getTokenSize() const {
        return m_activeTokenSize;
    }

    bool isSame( const char* token, std::size_t tokenSize ) const;

    bool isSame( const char* token ) const;

    bool isSameInOneCase( const char* token, std::size_t tokenSize ) const;

    bool isSameInOneCase( const char* token ) const;

    inline unsigned int getCurrentDepth() const {
        return m_depth;
    }

    void resetTokenProcessing();

    bool read( bool& value ) const;

    bool read( float& value ) const;

    bool read( int& value ) const;

    bool read( uint16_t& value ) const;

    bool read( IValue*& value ) const;

    bool readHex( uint32_t& value ) const;

protected:
    virtual bool processActiveToken() = 0;

    virtual bool onStepIn();

    virtual bool onStepOut();

    virtual void onSymbolParsed( char symbol );

    /**
     * @brief Called when token doesn't fit into buffer
     *
     * @return true Internal buffer will be cleared and filled with the following symbols
     * @return false Internal buffer will hold trancated value and the rest symbols will be discarded
     */
    virtual bool onLongToken();

private:
    bool isActiveBlockNeededToParse() const;

    inline void setTokenTruncated() {
        m_stateFlags |= Flags::TokenTruncated;
    }

    inline void enableQuotedTokenProcessing() {
        m_stateFlags |= Flags::QuotedToken;
    }

    inline void enableFloatTokenProcessing() {
        m_stateFlags |= Flags::FloatToken;
    }

    inline void enableNotQuotedTokenProcessing() {
        m_stateFlags |= Flags::NotQuotedToken;
    }

    inline void enableValueTokenProcessing() {
        m_stateFlags |= Flags::Value;
    }

    inline void disableValueTokenProcessing() {
        m_stateFlags ^= Flags::Value;
    }

    bool processActiveTokenAndResetState();

    void addCharToToken( char value );

    void goHigher();

    void goDeeper();

private:
    std::uint8_t m_stateFlags = Flags::Empty;

    std::size_t m_depth = 0;
    std::size_t m_ignoreLevel = 0;

    std::size_t m_activeTokenSize = 0;
    std::size_t m_maxBufferSize = 0;
    char* m_buffer = nullptr;
};

}  // namespace common
