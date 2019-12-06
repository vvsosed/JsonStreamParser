
#pragma once

#include <cstddef>

namespace jssp {

template<typename String>
class JsonListenerBase {
  public:
    virtual ~JsonListenerBase() {};
    
    virtual void whitespace(char c) {};
  
    virtual void startDocument() {};

    virtual void key(String key) {};

    virtual void value(String value) {};

    virtual void value(int value) {};

    virtual void value(float value) {};

    virtual void value(bool value) {};

    virtual void value(std::nullptr_t) {};

    virtual void endArray() {};

    virtual void endObject() {};

    virtual void endDocument() {};

    virtual void startArray() {};

    virtual void startObject() {};

    virtual void error(int code) {};
};

} // namespace jssp
