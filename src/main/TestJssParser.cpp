#include "TestJssParser.h"
#include "IReadStream.h"
#include "JsonStreamGenerator.h"

#include <iostream>

namespace jssp {

JsonPrinter::JsonPrinter( const JsonStreamingParser& parser )
: m_parser(parser) {}

void JsonPrinter::key(String key) {
	std::cout << m_prefix << key << ":";
}

void JsonPrinter::value(String value) {
	std::cout << m_prefix << '"' << value << "\",\n";
}

void JsonPrinter::value(int value) {
	std::cout << m_prefix << value << ",\n";
}

void JsonPrinter::value(float value) {
	std::cout << m_prefix << value << ",\n";
}

void JsonPrinter::value(bool value) {
	std::cout << m_prefix << (value ? "true" : "false") << ",\n";
}

void JsonPrinter::value(std::nullptr_t) {
	std::cout << m_prefix << "null,\n";
}

void JsonPrinter::startArray() {
	std::cout << m_prefix << "[\n";
	m_prefix = std::string(m_parser.depth(), ' ');
}

void JsonPrinter::endArray() {
	m_prefix = std::string(m_parser.depth(), ' ');
	std::cout << m_prefix << "],\n";
}

void JsonPrinter::startObject() {
	std::cout << m_prefix << "{\n";
	m_prefix = std::string(m_parser.depth(), ' ');
}

void JsonPrinter::endObject() {
	m_prefix = std::string(m_parser.depth(), ' ');
	std::cout << m_prefix << "},\n";
}

void testJsonPrinter( common::IReadStream& rStream ) {
	std::cout << "\n---------- Run testJsonPrinter ..." << std::endl;
	JsonStreamingParser jsParser;
	JsonPrinter jsPrinter(jsParser);
	jsParser.setListener(&jsPrinter);
	jsParser.parse(rStream);
	auto isSuccess = false;
	std::cout << "---------- Complete testJsonPrinter\n" << std::endl;
}

} // namespace jssp


