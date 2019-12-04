
#pragma once

#include <string>

namespace jssp {

using String = std::string;

class JsonListener {
  public:
    
    virtual void whitespace(char c) = 0;
  
    virtual void startDocument() = 0;

    virtual void key(String key) = 0;

    virtual void value(String value) = 0;

    virtual void endArray() = 0;

    virtual void endObject() = 0;

    virtual void endDocument() = 0;

    virtual void startArray() = 0;

    virtual void startObject() = 0;
};

} // namespace jssp
