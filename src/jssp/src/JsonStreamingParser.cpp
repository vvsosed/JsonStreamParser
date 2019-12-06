#include <algorithm>
#include <cmath>
#include <string>

#include "JsonStreamingParser.h"
#include "IReadStream.h"
#include "StlBlockTypes.h"

namespace jssp {

template <typename String>
JsonStreamingParser<String>::JsonStreamingParser() {
    reset();
}

template <typename String>
void JsonStreamingParser<String>::reset() {
    m_state = STATE_START_DOCUMENT;
    bufferPos = 0;
    unicodeEscapeBufferPos = 0;
    unicodeBufferPos = 0;
    characterCounter = 0;
}

template <typename String>
void JsonStreamingParser<String>::setListener(JsonListener* listener) {
  myListener = listener;
}

template <typename String>
void JsonStreamingParser<String>::parse(char c) {
    //System.out.print(c);
    // valid whitespace characters in JSON (from RFC4627 for JSON) include:
    // space, horizontal tab, line feed or new line, and carriage return.
    // thanks:
    // http://stackoverflow.com/questions/16042274/definition-of-whitespace-in-json
    if ((c == ' ' || c == '\t' || c == '\n' || c == '\r')
        && !(m_state == STATE_IN_STRING || m_state == STATE_UNICODE || m_state == STATE_START_ESCAPE
            || m_state == STATE_IN_NUMBER || m_state == STATE_START_DOCUMENT)) {
      return;
    }
    switch (m_state) {
    case STATE_IN_STRING:
      if (c == '"') {
        endString();
      } else if (c == '\\') {
        m_state = STATE_START_ESCAPE;
      } else if ((c < 0x1f) || (c == 0x7f)) {
        //throw new RuntimeException("Unescaped control character encountered: " + c + " at position" + characterCounter);
      } else {
        buffer[bufferPos] = c;
        increaseBufferPointer();
      }
      break;
    case STATE_IN_ARRAY:
      if (c == ']') {
        endArray();
      } else {
        startValue(c);
      }
      break;
    case STATE_IN_OBJECT:
      if (c == '}') {
        endObject();
      } else if (c == '"') {
        startKey();
      } else {
        //throw new RuntimeException("Start of string expected for object key. Instead got: " + c + " at position" + characterCounter);
      }
      break;
    case STATE_END_KEY:
      if (c != ':') {
        //throw new RuntimeException("Expected ':' after key. Instead got " + c + " at position" + characterCounter);
      }
      m_state = STATE_AFTER_KEY;
      break;
    case STATE_AFTER_KEY:
      startValue(c);
      break;
    case STATE_START_ESCAPE:
      processEscapeCharacters(c);
      break;
    case STATE_UNICODE:
      processUnicodeCharacter(c);
      break;
    case STATE_UNICODE_SURROGATE:
      unicodeEscapeBuffer[unicodeEscapeBufferPos] = c;
      unicodeEscapeBufferPos++;
      if (unicodeEscapeBufferPos == 2) {
        endUnicodeSurrogateInterstitial();
      }
      break;
    case STATE_AFTER_VALUE: {
      // not safe for size == 0!!!
      int within = stack[stackPos - 1];
      if (within == STACK_OBJECT) {
        if (c == '}') {
          endObject();
        } else if (c == ',') {
          m_state = STATE_IN_OBJECT;
        } else {
          //throw new RuntimeException("Expected ',' or '}' while parsing object. Got: " + c + ". " + characterCounter);
        }
      } else if (within == STACK_ARRAY) {
        if (c == ']') {
          endArray();
        } else if (c == ',') {
          m_state = STATE_IN_ARRAY;
        } else {
          //throw new RuntimeException("Expected ',' or ']' while parsing array. Got: " + c + ". " + characterCounter);

        }
      } else {
        //throw new RuntimeException("Finished a literal, but unclear what state to move to. Last state: " + characterCounter);
      }
    }break;
    case STATE_IN_NUMBER:
      if (c >= '0' && c <= '9') {
        buffer[bufferPos] = c;
        increaseBufferPointer();
      } else if (c == '.') {
        if (doesCharArrayContain(buffer, bufferPos, '.')) {
          //throw new RuntimeException("Cannot have multiple decimal points in a number. " + characterCounter);
        } else if (doesCharArrayContain(buffer, bufferPos, 'e')) {
          //throw new RuntimeException("Cannot have a decimal point in an exponent." + characterCounter);
        }
        buffer[bufferPos] = c;
        increaseBufferPointer();
      } else if (c == 'e' || c == 'E') {
        if (doesCharArrayContain(buffer, bufferPos, 'e')) {
          //throw new RuntimeException("Cannot have multiple exponents in a number. " + characterCounter);
        }
        buffer[bufferPos] = c;
        increaseBufferPointer();
      } else if (c == '+' || c == '-') {
        char last = buffer[bufferPos - 1];
        if (!(last == 'e' || last == 'E')) {
          //throw new RuntimeException("Can only have '+' or '-' after the 'e' or 'E' in a number." + characterCounter);
        }
        buffer[bufferPos] = c;
        increaseBufferPointer();
      } else {
        endNumber();
        // we have consumed one beyond the end of the number
        parse(c);
      }
      break;
    case STATE_IN_TRUE:
      buffer[bufferPos] = c;
      increaseBufferPointer();
      if (bufferPos == 4) {
        endTrue();
      }
      break;
    case STATE_IN_FALSE:
      buffer[bufferPos] = c;
      increaseBufferPointer();
      if (bufferPos == 5) {
        endFalse();
      }
      break;
    case STATE_IN_NULL:
      buffer[bufferPos] = c;
      increaseBufferPointer();
      if (bufferPos == 4) {
        endNull();
      }
      break;
    case STATE_START_DOCUMENT:
      myListener->startDocument();
      if (c == '[') {
        startArray();
      } else if (c == '{') {
        startObject();
      } else {
        // throw new ParsingError($this->_line_number,
        // $this->_char_number,
        // "Document must start with object or array.");
      }
      break;
    //case STATE_DONE:
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Expected end of document.");
    //default:
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Internal error. Reached an unknown state: ".$this->_state);
    }
    characterCounter++;
  }

template <typename String>
void JsonStreamingParser<String>::parse(common::IReadStream& rStream) {
	char ch;
	while( rStream.read(&ch, 1) ) {
		parse(ch);
	}
}

template <typename String>
void JsonStreamingParser<String>::increaseBufferPointer() {
  bufferPos = std::min(bufferPos + 1, BUFFER_MAX_LENGTH - 1);
}

template <typename String>
void JsonStreamingParser<String>::endString() {
    int popped = stack[stackPos - 1];
    stackPos--;
    if (popped == STACK_KEY) {
      buffer[bufferPos] = '\0';
      myListener->key(StringType(buffer));
      m_state = STATE_END_KEY;
    } else if (popped == STACK_STRING) {
      buffer[bufferPos] = '\0';
      myListener->value(StringType(buffer));
      m_state = STATE_AFTER_VALUE;
    } else {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Unexpected end of string.");
    }
    bufferPos = 0;
  }

template <typename String>
void JsonStreamingParser<String>::startValue(char c) {
    if (c == '[') {
      startArray();
    } else if (c == '{') {
      startObject();
    } else if (c == '"') {
      startString();
    } else if (isDigit(c)) {
      startNumber(c);
    } else if (c == 't') {
      m_state = STATE_IN_TRUE;
      buffer[bufferPos] = c;
      increaseBufferPointer();
    } else if (c == 'f') {
      m_state = STATE_IN_FALSE;
      buffer[bufferPos] = c;
      increaseBufferPointer();
    } else if (c == 'n') {
      m_state = STATE_IN_NULL;
      buffer[bufferPos] = c;
      increaseBufferPointer();
    } else {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Unexpected character for value: ".$c);
    }
  }

template <typename String>
bool JsonStreamingParser<String>::isDigit(char c) {
    // Only concerned with the first character in a number.
    return (c >= '0' && c <= '9') || c == '-';
  }

template <typename String>
void JsonStreamingParser<String>::endArray() {
    int popped = stack[stackPos - 1];
    stackPos--;
    if (popped != STACK_ARRAY) {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Unexpected end of array encountered.");
    }
    myListener->endArray();
    m_state = STATE_AFTER_VALUE;
    if (stackPos == 0) {
      endDocument();
    }
  }

template <typename String>
void JsonStreamingParser<String>::startKey() {
    stack[stackPos] = STACK_KEY;
    stackPos++;
    m_state = STATE_IN_STRING;
  }

template <typename String>
void JsonStreamingParser<String>::endObject() {
    int popped = stack[stackPos];
    stackPos--;
    if (popped != STACK_OBJECT) {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Unexpected end of object encountered.");
    }
    myListener->endObject();
    m_state = STATE_AFTER_VALUE;
    if (stackPos == 0) {
      endDocument();
    }
  }

template <typename String>
void JsonStreamingParser<String>::processEscapeCharacters(char c) {
    if (c == '"') {
      buffer[bufferPos] = '"';
      increaseBufferPointer();
    } else if (c == '\\') {
      buffer[bufferPos] = '\\';
      increaseBufferPointer();
    } else if (c == '/') {
      buffer[bufferPos] = '/';
      increaseBufferPointer();
    } else if (c == 'b') {
      buffer[bufferPos] = 0x08;
      increaseBufferPointer();
    } else if (c == 'f') {
      buffer[bufferPos] = '\f';
      increaseBufferPointer();
    } else if (c == 'n') {
      buffer[bufferPos] = '\n';
      increaseBufferPointer();
    } else if (c == 'r') {
      buffer[bufferPos] = '\r';
      increaseBufferPointer();
    } else if (c == 't') {
      buffer[bufferPos] = '\t';
      increaseBufferPointer();
    } else if (c == 'u') {
      m_state = STATE_UNICODE;
    } else {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Expected escaped character after backslash. Got: ".$c);
    }
    if (m_state != STATE_UNICODE) {
      m_state = STATE_IN_STRING;
    }
  }

template <typename String>
void JsonStreamingParser<String>::processUnicodeCharacter(char c) {
    if (!isHexCharacter(c)) {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Expected hex character for escaped Unicode character. Unicode parsed: "
      // . implode($this->_unicode_buffer) . " and got: ".$c);
    }

    unicodeBuffer[unicodeBufferPos] = c;
    unicodeBufferPos++;

    if (unicodeBufferPos == 4) {
      int codepoint = getHexArrayAsDecimal(unicodeBuffer, unicodeBufferPos);
      endUnicodeCharacter(codepoint);
      return;
      /*if (codepoint >= 0xD800 && codepoint < 0xDC00) {
        unicodeHighSurrogate = codepoint;
        unicodeBufferPos = 0;
        state = STATE_UNICODE_SURROGATE;
      } else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
        if (unicodeHighSurrogate == -1) {
          // throw new ParsingError($this->_line_number,
          // $this->_char_number,
          // "Missing high surrogate for Unicode low surrogate.");
        }
        int combinedCodePoint = ((unicodeHighSurrogate - 0xD800) * 0x400) + (codepoint - 0xDC00) + 0x10000;
        endUnicodeCharacter(combinedCodePoint);
      } else if (unicodeHighSurrogate != -1) {
        // throw new ParsingError($this->_line_number,
        // $this->_char_number,
        // "Invalid low surrogate following Unicode high surrogate.");
        endUnicodeCharacter(codepoint);
      } else {
        endUnicodeCharacter(codepoint);
      }*/
    }
  }

template <typename String>
bool JsonStreamingParser<String>::isHexCharacter(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  }

template <typename String>
int JsonStreamingParser<String>::getHexArrayAsDecimal(char hexArray[], int length) {
    int result = 0;
    for (int i = 0; i < length; i++) {
      char current = hexArray[length - i - 1];
      int value = 0;
      if (current >= 'a' && current <= 'f') {
        value = current - 'a' + 10;
      } else if (current >= 'A' && current <= 'F') {
        value = current - 'A' + 10;
      } else if (current >= '0' && current <= '9') {
        value = current - '0';
      }
      result += value * 16^i;
    }
    return result;
  }

template <typename String>
bool JsonStreamingParser<String>::doesCharArrayContain(char myArray[], int length, char c) {
    for (int i = 0; i < length; i++) {
      if (myArray[i] == c) {
        return true;
      }
    }
    return false;
  }

template <typename String>
void JsonStreamingParser<String>::endUnicodeSurrogateInterstitial() {
    char unicodeEscape = unicodeEscapeBuffer[unicodeEscapeBufferPos - 1];
    if (unicodeEscape != 'u') {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Expected '\\u' following a Unicode high surrogate. Got: " .
      // $unicode_escape);
    }
    unicodeBufferPos = 0;
    unicodeEscapeBufferPos = 0;
    m_state = STATE_UNICODE;
  }

template <typename String>
void JsonStreamingParser<String>::endNumber() {
    buffer[bufferPos] = '\0';
    StringType value = StringType(buffer);
    {

    	const char *cstrBegin = value.c_str();
    	char *cstrEnd;
    	int ival = std::strtol (cstrBegin, &cstrEnd, 10);
    	typename String::size_type sz = cstrEnd - cstrBegin;
    	if (sz == value.length()) {
    		myListener->value(ival);
    	}
    	else {
    		float fval = std::strtof (cstrBegin, &cstrEnd);
    		myListener->value(fval);
    	}
    }

    bufferPos = 0;
    m_state = STATE_AFTER_VALUE;
  }

template <typename String>
int JsonStreamingParser<String>::convertDecimalBufferToInt(char myArray[], int length) {
    int result = 0;
    for (int i = 0; i < length; i++) {
      char current = myArray[length - i - 1];
      result += (current - '0') * 10;
    }
    return result;
  }

template <typename String>
void JsonStreamingParser<String>::endDocument() {
    myListener->endDocument();
    m_state = STATE_DONE;
  }

template <typename String>
void JsonStreamingParser<String>::endTrue() {
    buffer[bufferPos] = '\0';
    StringType value = StringType(buffer);
    if (value == "true") {
      myListener->value(true);
    } else {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Expected 'true'. Got: ".$true);
    }
    bufferPos = 0;
    m_state = STATE_AFTER_VALUE;
  }

template <typename String>
void JsonStreamingParser<String>::endFalse() {
    buffer[bufferPos] = '\0';
    StringType value = StringType(buffer);
    if (value == "false") {
      myListener->value(false);
    } else {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Expected 'true'. Got: ".$true);
    }
    bufferPos = 0;
    m_state = STATE_AFTER_VALUE;
  }

template <typename String>
void JsonStreamingParser<String>::endNull() {
    buffer[bufferPos] = '\0';
    StringType value = StringType(buffer);
    if (value == "null") {
      myListener->value(nullptr);
    } else {
      // throw new ParsingError($this->_line_number, $this->_char_number,
      // "Expected 'true'. Got: ".$true);
    }
    bufferPos = 0;
    m_state = STATE_AFTER_VALUE;
  }

template <typename String>
void JsonStreamingParser<String>::startArray() {
    myListener->startArray();
    m_state = STATE_IN_ARRAY;
    stack[stackPos] = STACK_ARRAY;
    stackPos++;
  }

template <typename String>
void JsonStreamingParser<String>::startObject() {
    myListener->startObject();
    m_state = STATE_IN_OBJECT;
    stack[stackPos] = STACK_OBJECT;
    stackPos++;
  }

template <typename String>
void JsonStreamingParser<String>::startString() {
    stack[stackPos] = STACK_STRING;
    stackPos++;
    m_state = STATE_IN_STRING;
  }

template <typename String>
void JsonStreamingParser<String>::startNumber(char c) {
    m_state = STATE_IN_NUMBER;
    buffer[bufferPos] = c;
    increaseBufferPointer();
  }

template <typename String>
void JsonStreamingParser<String>::endUnicodeCharacter(int codepoint) {
    buffer[bufferPos] = convertCodepointToCharacter(codepoint);
    increaseBufferPointer();
    unicodeBufferPos = 0;
    unicodeHighSurrogate = -1;
    m_state = STATE_IN_STRING;
  }

template <typename String>
char JsonStreamingParser<String>::convertCodepointToCharacter(int num) {
    if (num <= 0x7F)
      return (char) (num);
    // if(num<=0x7FF) return (char)((num>>6)+192) + (char)((num&63)+128);
    // if(num<=0xFFFF) return
    // chr((num>>12)+224).chr(((num>>6)&63)+128).chr((num&63)+128);
    // if(num<=0x1FFFFF) return
    // chr((num>>18)+240).chr(((num>>12)&63)+128).chr(((num>>6)&63)+128).chr((num&63)+128);
    return ' ';
  }

template class JsonStreamingParser<std::string>;
template class JsonStreamingParser<common::block_string>;

} // namespace jssp
