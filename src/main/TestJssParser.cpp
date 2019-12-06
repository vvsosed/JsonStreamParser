#include "TestJssParser.h"
#include "IReadStream.h"
#include "JsonStreamGenerator.h"

#include <iostream>
#include <functional>

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

namespace {

class ItemsFilterFinderStateBase : public JsonListener {
public:
	ItemsFilterFinderStateBase( ItemsFilterFinder* finder )
	: m_finder(*finder) {}

	virtual ~ItemsFilterFinderStateBase() = default;

	inline auto& finder() {
		return m_finder;
	}

private:
	ItemsFilterFinder& m_finder;
};

class WaitStartOfTemplateBlock : public ItemsFilterFinderStateBase {
public:
	WaitStartOfTemplateBlock( ItemsFilterFinder* finder, const String& blockName )
	: ItemsFilterFinderStateBase(finder)
	, m_blockName(blockName) {}

	void key(String key) override {
		if (1 == finder().parserDepth() && m_blockName == key) {
			finder().OnTemplateBlockWasFounded();
		}
	};

private:
	const String m_blockName;
};

class WaitStartOfBlock : public ItemsFilterFinderStateBase {
public:
	WaitStartOfBlock(ItemsFilterFinder* finder)
	: ItemsFilterFinderStateBase(finder) {}

	void key(String key) override {
		if ( key == "item_filter" ) {
			finder().OnItemFilterBlockWasFounded();
		}
		else if ( key == "fields") {
			finder().OnFieldsBlockWasFounded();
		}
	};

	void startObject() override {
		if (finder().parserDepth() == finder().blockDepth()) {
			finder().OnTemplateStart();
		}
	};

	void endObject() override {
		if (finder().parserDepth() == finder().blockDepth()) {
			finder().OnTemplateEnd();
		}
	};

	void endArray() override {
		if (finder().parserDepth() == finder().blockDepth() - 1) {
			finder().OnCompleted();
		}
	};
};

class WaitNamesField : public ItemsFilterFinderStateBase {
public:
	using ItemsFilterFinderStateBase::ItemsFilterFinderStateBase;

	void key(String key) override {
		if ( key == "names" ) {
			finder().OnNamesFieldWasFounded();
		}
	}
};

class ReadItemsFilterValues : public ItemsFilterFinderStateBase {
public:
	using TokensSet = ItemsFilterFinder::TokensSet;

	using ItemsFilterFinderStateBase::ItemsFilterFinderStateBase;

	void key(String key) override {
		finder().OnFailed("Unexpected key in ReadItemsFilterValues");
	};

	void value(String value) override {
		if ( m_isArrayStarted ) {
			m_tokens.emplace(value);
		}
		else {
			finder().OnFailed("Value should be array in ReadItemsFilterValues.");
		}
	};

	void value(int value) override {
		finder().OnFailed("Unexpected int value in ReadItemsFilterValues. Only strings is supported!");
	};

	void value(float value) override {
		finder().OnFailed("Unexpected float value in ReadItemsFilterValues. Only strings is supported!");
	};

	void value(bool value) override {
		finder().OnFailed("Unexpected bool value in ReadItemsFilterValues. Only strings is supported!");
	};

	void value(std::nullptr_t) override {
		finder().OnFailed("Unexpected nullptr value in ReadItemsFilterValues. Only strings is supported!");
	};

	void startArray() override {
		if (m_isArrayStarted) {
			finder().OnFailed("Unexpected array value in ReadItemsFilterValues. Only strings is supported!");
		}
		else {
			m_isArrayStarted = true;
		}
	};

	void endArray() override {
		if ( m_isArrayStarted ) {
			finder().OnItemFiltersValuesWasReaded(std::move(m_tokens));
		}
		else {
			finder().OnFailed("Value should be array in ReadItemsFilterValues.");
		}
	};

	void startObject() override {
		finder().OnFailed("Unexpected start object in ReadItemsFilterValues");
	};

	void endObject() override {
		finder().OnFailed("Unexpected end object in ReadItemsFilterValues");
	};

private:
	bool m_isArrayStarted = false;
	TokensSet m_tokens;
};

class FindItemBlock : public ItemsFilterFinderStateBase {
public:
	using ItemsFilterFinderStateBase::ItemsFilterFinderStateBase;

	void key(String key) override {
		m_key = key;
	};

	void value(String value) override {
		if (m_key == "name" && value == "item") {
			m_isNameValid = true;
		}
		else if (m_key == "type" && value == "item") {
			m_isTypeValid = true;
		}
		else if (m_key == "value" && value == "value") {
			m_isValueValid = true;
		}
		else {
			m_key.clear();
		}
	}

	void startObject() override {
		m_start = finder().streamPos();
	};

	void endObject() override {
		if (m_isNameValid && m_isTypeValid && m_isValueValid) {
			finder().OnItemBlockWasFounded(m_start, finder().streamPos() + 1);
		}
		else {
			m_isNameValid = false;
		    m_isTypeValid = false;
			m_isValueValid = false;
		}
	};

private:
	String m_key;
	bool m_isNameValid = false,
		 m_isTypeValid = false,
		 m_isValueValid = false;
	int m_start;
};

class WaitingEndOfBlock : public ItemsFilterFinderStateBase {
public:
	using ItemsFilterFinderStateBase::ItemsFilterFinderStateBase;
};

class Completed : public ItemsFilterFinderStateBase {
public:
	using ItemsFilterFinderStateBase::ItemsFilterFinderStateBase;
};

class Failed : public ItemsFilterFinderStateBase {
public:
	using ItemsFilterFinderStateBase::ItemsFilterFinderStateBase;
};

} // private namespace

ItemsFilterFinder::ItemsFilterFinder( const String& field )
: m_field(field) {};

template<typename S, typename... Args>
inline void ItemsFilterFinder::setState( Args... args ) {
	m_state2 = std::make_unique<S>(this, args...);
	m_parser.setListener(m_state2.get());
}

std::tuple<bool, ItemsFilterFinder::DataList> ItemsFilterFinder::find( common::IReadStream& rStream ) {
	/*JsonStreamingParser jsParser;
	JsonPrinter jsPrinter(jsParser);
	jsParser.setListener(&jsPrinter);*/

	setState<WaitStartOfTemplateBlock>(m_field);

	char ch;
	while( rStream.read(&ch, 1) ) {
		//jsParser.parse(ch);

		m_parser.parse(ch);
		if ( State::InWork != m_state ) {
			break;
		}
		++m_pos;
	}
	return std::make_tuple(State::Completed == m_state, std::move(m_dataList));
}

void ItemsFilterFinder::OnFailed( String&& reason ) {
	m_state = State::Failed;
}

void ItemsFilterFinder::OnCompleted() {
	m_state = State::Completed;
}

void ItemsFilterFinder::OnTemplateBlockWasFounded() {
	setState<WaitStartOfBlock>();
	m_blockDept = m_parser.depth() + 1;
}

void ItemsFilterFinder::OnTemplateStart() {
	m_bData.m_start = m_pos;
}

void ItemsFilterFinder::OnTemplateEnd() {
	m_bData.m_end = m_pos + 1;
	if ( m_bData.isValid()) {
		m_dataList.emplace_back(std::move(m_bData));
	}
	m_bData.reset();
}

void ItemsFilterFinder::OnItemFilterBlockWasFounded() {
	setState<WaitNamesField>();
}

void ItemsFilterFinder::OnFieldsBlockWasFounded() {
	setState<FindItemBlock>();
}

void ItemsFilterFinder::OnItemBlockWasFounded(const int blockStart, const int blockEnd) {
	m_bData.m_itemBlockStart = blockStart;
	m_bData.m_itemBlockEnd = blockEnd;
	setState<WaitStartOfBlock>();
}

void ItemsFilterFinder::OnNamesFieldWasFounded() {
	setState<ReadItemsFilterValues>();
}

void ItemsFilterFinder::OnItemFiltersValuesWasReaded( TokensSet&& tokens ) {
	m_bData.m_tokens = std::move(tokens);
	setState<WaitStartOfBlock>();
}

//-------------------------------------------------------------------------------

TemplatesGenerator::TemplatesGenerator( common::JsonStreamGenerator& gen )
: m_gen(gen) {
	m_parser.setListener(this);
};

void TemplatesGenerator::key(String key) {
	m_key = key;
	m_isSuccess = m_gen.writePropertyKey(key.c_str());
};

void TemplatesGenerator::value(String value) {
	if ( value == "value" &&
		 m_key == "value" &&
		 m_data->m_itemBlockStart < m_pos &&
		 m_data->m_itemBlockEnd > m_pos) {
		value = m_currItem;
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
		m_currItem = item;

		for (const auto& data : dataList) {
			if ( !checkFilter(item, data.m_tokens) ) {
				continue;
			}
			m_data = &data;

			m_isSuccess = rStream.reset(data.m_start);
			if ( !m_isSuccess ) {
				std::cerr << "Can't reset stream!" << std::endl;
				break;
			}

			m_pos = data.m_start;
			while( m_pos != data.m_end ) {
				char ch;
				m_isSuccess = (1 == rStream.read(&ch, 1));
				if ( !m_isSuccess ) {
					std::cerr << "Can't read char from stream!" << std::endl;
					break;
				}
				m_parser.parse(ch);
				++m_pos;
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

} // namespace jssp


