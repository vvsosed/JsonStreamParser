#include "TestJssParser.h"
#include "IReadStream.h"
#include "JsonStreamGenerator.h"

#include <iostream>

namespace jssp {

JsonPrinter::JsonPrinter( const JsonStreamingParser& parser )
: m_parser(parser) {}

void JsonPrinter::key(String key) {
	std::cout << m_prefix << key << ":";
	std::cout.flush();
}

void JsonPrinter::value(String value) {
	std::cout << m_prefix << '"' << value << "\",\n";
	std::cout.flush();
}

void JsonPrinter::value(int value) {
	std::cout << m_prefix << value << ",\n";
	std::cout.flush();
}

void JsonPrinter::value(float value) {
	std::cout << m_prefix << value << ",\n";
	std::cout.flush();
}

void JsonPrinter::value(bool value) {
	std::cout << m_prefix << (value ? "true" : "false") << ",\n";
	std::cout.flush();
}

void JsonPrinter::value(std::nullptr_t) {
	std::cout << m_prefix << "null,\n";
}

void JsonPrinter::startArray() {
	std::cout << m_prefix << "[\n";
	std::cout.flush();
	m_prefix = std::string(m_parser.depth(), ' ');
}

void JsonPrinter::endArray() {
	m_prefix = std::string(m_parser.depth(), ' ');
	std::cout << m_prefix << "],\n";
	std::cout.flush();
}

void JsonPrinter::startObject() {
	std::cout << m_prefix << "{\n";
	std::cout.flush();
	m_prefix = std::string(m_parser.depth(), ' ');
}

void JsonPrinter::endObject() {
	m_prefix = std::string(m_parser.depth(), ' ');
	std::cout << m_prefix << "},\n";
	std::cout.flush();
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

ItemsFilterFinder::ItemsFilterFinder( const String& field )
: m_field(field) {
	m_parser.setListener(this);
};

void ItemsFilterFinder::key(String key) {
	switch(m_state) {
	case State::WaitStartOfTemplatesBlock:
		if (key == m_field) {
			m_state = State::WaitStartOfBlock;
			m_blockDept = m_parser.depth() + 1;
		}
		break;
	case State::WaitStartOfBlock:
		m_state = State::Failed;
		break;
	case State::WaitItemFilterFieldKey:
		if (key == "item_filter") {
			m_state = State::WaitNamesFieldKey;
		}
		break;
	case State::WaitNamesFieldKey:
		if ( key == "names") {
			m_state = State::WaitArrayBegin;
		}
		break;
	case State::WaitArrayBegin:
		m_state = State::Failed;
		break;
	case State::ReadItems:
		m_state = State::Failed;
		break;
	case State::WaitingEndOfBlock:
		break;
	default:
		break;
	}
}

void ItemsFilterFinder::value(String value) {
	if (m_state == State::ReadItems) {
		m_bData.m_tokens.emplace(std::move(value));
	}
}

void ItemsFilterFinder::startArray() {
	switch(m_state) {
	case State::WaitStartOfTemplatesBlock:
		break;
	case State::WaitStartOfBlock:
		break;
	case State::WaitItemFilterFieldKey:
		break;
	case State::WaitNamesFieldKey:
		break;
	case State::WaitArrayBegin:
		m_state = State::ReadItems;
		break;
	case State::ReadItems:
		m_state = State::Failed;
		break;
	case State::WaitingEndOfBlock:
		break;
	default:
		break;
	}
}

void ItemsFilterFinder::endArray() {
	switch(m_state) {
	case State::WaitStartOfTemplatesBlock:
		break;
	case State::WaitStartOfBlock:
		if (m_parser.depth() == m_blockDept - 1) {
			m_state = State::Completed;
		}
		break;
	case State::WaitItemFilterFieldKey:
		break;
	case State::WaitNamesFieldKey:
		break;
	case State::WaitArrayBegin:
		m_state = State::Failed;
		break;
	case State::ReadItems:
		m_state = State::WaitingEndOfBlock;
		break;
	case State::WaitingEndOfBlock:
		break;
	default:
		break;
	}
}

void ItemsFilterFinder::startObject() {
	switch(m_state) {
	case State::WaitStartOfTemplatesBlock:
		break;
	case State::WaitStartOfBlock:
		m_state = State::WaitItemFilterFieldKey;
		m_bData.m_start = m_pos;
		break;
	case State::WaitItemFilterFieldKey:
		break;
	case State::WaitNamesFieldKey:
		break;
	case State::WaitArrayBegin:
		m_state = State::Failed;
		break;
	case State::ReadItems:
		m_state = State::Failed;
		break;
	case State::WaitingEndOfBlock:
		break;
	default:
		break;
	}
}

void ItemsFilterFinder::endObject() {
	switch(m_state) {
	case State::WaitStartOfTemplatesBlock:
		break;
	case State::WaitStartOfBlock:
		break;
	case State::WaitItemFilterFieldKey:
		break;
	case State::WaitNamesFieldKey:
		break;
	case State::WaitArrayBegin:
		m_state = State::Failed;
		break;
	case State::ReadItems:
		m_state = State::Failed;
		break;
	case State::WaitingEndOfBlock:
		if (m_parser.depth() == m_blockDept) {
			m_bData.m_end = m_pos + 1;
			m_dataList.emplace_back(std::move(m_bData));
			m_bData.reset();
			m_state = State::WaitStartOfBlock;
		}
		break;
	default:
		break;
	}
}

std::tuple<bool, ItemsFilterFinder::DataList> ItemsFilterFinder::find( common::IReadStream& rStream ) {
	/*JsonStreamingParser jsParser;
	JsonPrinter jsPrinter(jsParser);
	jsParser.setListener(&jsPrinter);*/

	char ch;
	while( rStream.read(&ch, 1) ) {
		//jsParser.parse(ch);

		m_parser.parse(ch);
		if ( State::Completed == m_state || State::Failed == m_state ) {
			break;
		}
		++m_pos;
	}
	return std::make_tuple(State::Completed == m_state, std::move(m_dataList));
}

TemplatesGenerator::TemplatesGenerator( common::JsonStreamGenerator& gen )
: m_gen(gen) {
	m_parser.setListener(this);
};

void TemplatesGenerator::key(String key) {
	m_key = key;
	m_isSuccess = m_gen.writePropertyKey(key.c_str());
};

void TemplatesGenerator::value(String value) {
	if (value == "item") {
		if (m_key == "name") {
			m_isItemName = true;
		}
		else if (m_key == "type") {
			m_isItemType = true;
		}
	}
	else if (value == "value" && m_key == "value" && m_isItemName && m_isItemType) {
		value = m_itemValue;
	}
	m_isSuccess = m_gen.writeProperty(nullptr, value);
};

void TemplatesGenerator::value(int value) {
	m_isSuccess = m_gen.writeProperty(nullptr, value);
};

void TemplatesGenerator::value(float value) {
	m_isSuccess = m_gen.writeProperty(nullptr, value);
};

void TemplatesGenerator::value(bool value) {
	m_isSuccess = m_gen.writeProperty(nullptr, value);
};

void TemplatesGenerator::value(std::nullptr_t) {
	m_isSuccess = m_gen.writeProperty(nullptr, nullptr);
};

void TemplatesGenerator::endArray() {
	m_key.clear();
	m_isSuccess = m_gen.writeClosingSymbol();
};

void TemplatesGenerator::endObject() {
	m_key.clear();
	m_isSuccess = m_gen.writeClosingSymbol();
	resetItemBlockFlags();
};

void TemplatesGenerator::startArray() {
	m_key.clear();
	m_isSuccess = m_gen.writeOpeningArray();
};

void TemplatesGenerator::startObject() {
	m_key.clear();
	m_isSuccess = m_gen.writeOpeningObject();
};

bool TemplatesGenerator::generate( const std::list<std::string> itemsList,
		                           const DataList& dataList,
								   common::IReadStream& rStream ) {
	for (const auto& item : itemsList) {
		for (const auto& data : dataList) {
			if ( !checkFilter(item, data.m_tokens) ) {
				continue;
			}
			m_itemValue = item;

			m_isSuccess = rStream.reset(data.m_start);
			if ( !m_isSuccess ) {
				std::cerr << "Can't reset stream!" << std::endl;
				break;
			}

			auto bytesCount = data.m_end - data.m_start;
			while( bytesCount-- ) {
				char ch;
				m_isSuccess = (1 == rStream.read(&ch, 1));
				if ( !m_isSuccess ) {
					std::cerr << "Can't read char from stream!" << std::endl;
					break;
				}
				m_parser.parse(ch);
			}

			m_parser.reset();
		}
	}
	return m_isSuccess;
}

bool TemplatesGenerator::checkFilter( const TokenType& token, const TokensSet& filtersSet ) const {
	for (const auto& filter : filtersSet) {
		if (token.substr(0, filter.length()) == filter) {
			return true;
		}
	}
	return false;
}

void TemplatesGenerator::resetItemBlockFlags() {
	m_isItemName = false;
	m_isItemType = false;
}

} // namespace jssp


