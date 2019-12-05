#pragma once

#include "JsonStreamingParser.h"
#include "JsonListener.h"

namespace common {
class IReadStream;
class JsonStreamGenerator;
} // namespace common

namespace jssp {

class JsonPrinter : public JsonListener {
	String m_prefix;
	const JsonStreamingParser& m_parser;
public:
	JsonPrinter( const JsonStreamingParser& parser );

	void key(String key) override;

	void value(String value) override;

	void value(int value) override;

	void value(float value) override;

	void value(bool value) override;

	void value(std::nullptr_t) override;

	void endArray() override;

	void endObject() override;

	void startArray() override;

	void startObject() override;
};

void testJsonPrinter( common::IReadStream& rStream );

} // namespace jssp
