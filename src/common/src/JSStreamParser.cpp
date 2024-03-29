#include "../include/JSStreamParser.h"

#include <values/StringValue.h>
#include <values/IntValue.h>
#include <values/BoolValue.h>
#include <values/FloatValue.h>

#include "IReadStream.h"

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <cinttypes>

#include <DebugUtils.h>

namespace common {
namespace {
constexpr char TrueBoolValue[] = "true";
constexpr char FalseBoolValue[] = "false";
}  // namespace

JSStreamParser::JSStreamParser( char* buffer, std::size_t size )
: m_maxBufferSize( size )
, m_buffer( buffer ) {}

void JSStreamParser::addCharToToken( char value ) {
    if( !isActiveBlockNeededToParse() ) {
        return;
    }

    if( m_activeTokenSize < m_maxBufferSize ) {
        m_buffer[m_activeTokenSize++] = value;
    }
    else if( m_activeTokenSize == m_maxBufferSize && onLongToken() ) {
        m_activeTokenSize = 0;
        m_buffer[m_activeTokenSize++] = value;
    }
    else {
        setTokenTruncated();
    }
}

bool JSStreamParser::isSame( const char* token, std::size_t tokenSize ) const {
    return ( tokenSize == m_activeTokenSize ) && ( strncmp( m_buffer, token, m_activeTokenSize ) == 0 );
}

bool JSStreamParser::isSameInOneCase( const char* token, std::size_t tokenSize ) const {
    return ( tokenSize == m_activeTokenSize ) && ( strncasecmp( m_buffer, token, m_activeTokenSize ) == 0 );
}

bool JSStreamParser::isSame( const char* token ) const {
    return strcmp( m_buffer, token ) == 0;
}

bool JSStreamParser::isSameInOneCase( const char* token ) const {
    return strcasecmp( m_buffer, token ) == 0;
}

bool JSStreamParser::read( IValue*& value ) const {
    FUNC_LOGW( "To value: %s", getTokenValue() );
    if( isNotQuotedTokenProcessing() ) {
        if( isFloatTokenProcessing() ) {
            float floatValue = 0;
            if( read( floatValue ) ) {
                value = new FloatValue( floatValue );
                return true;
            }
        }

        bool boolValue = false;
        if( read( boolValue ) ) {
            value = new BoolValue( boolValue );
            return true;
        }

        int intValue = 0;
        if( read( intValue ) ) {
            value = new IntValue( intValue );
            return true;
        }
    }
    else {
        value = new StringValue( getTokenValue(), getTokenSize() );
        return true;
    }

    return false;
}

bool JSStreamParser::read( bool& value ) const {
    if( isNotQuotedTokenProcessing() && !isFloatTokenProcessing() ) {
        if( isSameInOneCase( TrueBoolValue, sizeof( TrueBoolValue ) ) ) {
            value = true;
            return true;
        }
        else if( isSameInOneCase( FalseBoolValue, sizeof( FalseBoolValue ) ) ) {
            value = false;
            return true;
        }
    }

    return false;
}

bool JSStreamParser::read( float& value ) const {
    return 1 == sscanf( getTokenValue(), "%f", &value );
}

bool JSStreamParser::read( int& value ) const {
    return 1 == sscanf( getTokenValue(), "%d", &value );
}

bool JSStreamParser::read( uint16_t& value ) const {
    return 1 == sscanf( getTokenValue(), "%" SCNu16, &value );
}

bool JSStreamParser::readHex( uint32_t& value ) const {
    if( 1 != sscanf( getTokenValue(), "%" PRIX32, &value ) ) {
        return ( 1 == sscanf( getTokenValue(), "%" PRIx32, &value ) );
    }

    return true;
}

bool JSStreamParser::isActiveBlockNeededToParse() const {
    return ( m_ignoreLevel == 0 ) || ( m_ignoreLevel > m_depth );
}

bool JSStreamParser::onLongToken() {
    // Keep trancated value by default
    return false;
}

bool JSStreamParser::parse( IReadStream& stream ) {
    while( stream.read( &m_currSymbol, 1 ) ) {
        onSymbolParsed();

        switch( m_currSymbol ) {
        case '{':
            if( isQuotedTokenProcessing() ) {
                addCharToToken( m_currSymbol );
                break;
            }

            if( isNotQuotedTokenProcessing() ) {
                if( !processActiveTokenAndResetState() ) {
                    return false;
                }
            }
            else if( isValueProcessing() ) {
                disableValueTokenProcessing();
            }

            if ( goDeeper() && !onObjectBegin() ) {
            	return false;
            }
            break;

        case '}':
		if( isQuotedTokenProcessing() ) {
			addCharToToken( m_currSymbol );
			break;
		}

		if( isNotQuotedTokenProcessing() ) {
			if( !processActiveTokenAndResetState() ) {
				return false;
			}
		}

		if ( goHigher() && !onObjectEnd() ) {
			return false;
		}
		break;

        case '[':
			if( isQuotedTokenProcessing() ) {
				addCharToToken( m_currSymbol );
				break;
			}

			if( isNotQuotedTokenProcessing() ) {
				if( !processActiveTokenAndResetState() ) {
					return false;
				}
			}
			else if( isValueProcessing() ) {
				disableValueTokenProcessing();
			}

			if ( goDeeper() && !onArrayBegin() ) {
				return false;
			}
			break;

        case ']':
			if( isQuotedTokenProcessing() ) {
				addCharToToken( m_currSymbol );
				break;
			}

			if( isNotQuotedTokenProcessing() ) {
				if( !processActiveTokenAndResetState() ) {
					return false;
				}
			}

			if ( goHigher() && !onArrayEnd() ) {
				return false;
			}
			break;

        case '\'':
        case '"':
            if( isQuotedTokenProcessing() ) {
                if( !processActiveTokenAndResetState() ) {
                    return false;
                }
            }
            else {
                enableQuotedTokenProcessing();
            }
            break;

        case ':':
            if( isQuotedTokenProcessing() ) {
                addCharToToken( m_currSymbol );
                break;
            }
            else if( isNotQuotedTokenProcessing() ) {
                if( !processActiveTokenAndResetState() ) {
                    return false;
                }
            }

            enableValueTokenProcessing();
            break;

        case ',':
            if( isQuotedTokenProcessing() ) {
                addCharToToken( m_currSymbol );
                break;
            }

            if( isNotQuotedTokenProcessing() ) {
                if( !processActiveTokenAndResetState() ) {
                    return false;
                }
            }

            if ( !onNext() ) {
            	return false;
            }
            break;

        case '.':
            if( isQuotedTokenProcessing() ) {
                addCharToToken( m_currSymbol );
            }
            else if( isNotQuotedTokenProcessing() ) {
                addCharToToken( m_currSymbol );
                enableFloatTokenProcessing();
            }
            break;

        case ' ':
        case '\t':
        case '\r':
        case '\n':
            if( isQuotedTokenProcessing() ) {
                addCharToToken( m_currSymbol );
            }
            else if( isNotQuotedTokenProcessing() ) {
                if( !processActiveTokenAndResetState() ) {
                    return false;
                }
            }
            break;

        default:
            if( isQuotedTokenProcessing() ) {
                addCharToToken( m_currSymbol );
            }
            else {
                enableNotQuotedTokenProcessing();
                addCharToToken( m_currSymbol );
            }
            break;
        }
    }

    return true;
}

void JSStreamParser::resetTokenProcessing() {
    m_activeTokenSize = 0;
    m_stateFlags = Flags::Empty;
}

bool JSStreamParser::processActiveTokenAndResetState() {
    auto result = true;

    if( isActiveBlockNeededToParse() ) {
        if( m_activeTokenSize < m_maxBufferSize ) {
            m_buffer[m_activeTokenSize++] = '\0';
        }

        result = processActiveToken();
    }

    resetTokenProcessing();
    return result;
}

bool JSStreamParser::goHigher() {
    if( m_depth > 0 ) {
        --m_depth;
    }

    if( m_ignoreLevel == 0 ) {
        return true;
    }
    else if( isActiveBlockNeededToParse() ) {
        m_ignoreLevel = 0;
        return true;
    }

    return false;
}

bool JSStreamParser::goDeeper() {
    ++m_depth;
    return isActiveBlockNeededToParse();
}

}  // namespace common
