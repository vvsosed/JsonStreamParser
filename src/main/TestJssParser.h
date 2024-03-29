#pragma once

#include "JsonStreamingParser.h"
#include "StlBlockTypes.h"

#include <tuple>
#include <memory>
#include <string>

namespace common {
class IReadStream;
class JsonStreamGenerator;
} // namespace common

namespace jssp {

class JsonPrinter : public JsonStreamingParser<std::string>::JsonListener {
public:
	using Parser = JsonStreamingParser<std::string>;
	using String = Parser::StringType;

	JsonPrinter( const Parser& parser );

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

private:
	String m_prefix;
	const Parser& m_parser;
};

void testJsonPrinter( common::IReadStream& rStream );

class ItemsFilterFinder {
public:
	//using String = std::string;
	using String = common::block_string;

	using TokenType = String;
	using TokensSet = common::block_set<TokenType>;
	struct BlockData {
		int m_start = -1, m_end = -1,
		    m_itemBlockStart = -1, m_itemBlockEnd = -1;
		TokensSet m_tokens;

		bool isValid() {
			return !m_tokens.empty() &&
				   0 <= m_start && 0 <= m_end &&
				   0 <= m_itemBlockStart && 0 <= m_itemBlockEnd;
		}

		void reset() {
			m_start = -1; m_end = -1;
			m_itemBlockStart = -1; m_itemBlockEnd = -1;
			m_tokens = TokensSet();
		}
	};

	using DataList = common::block_list<BlockData>;

	using Parser = JsonStreamingParser<String>;
	using ParserListener = Parser::JsonListener;
	using StateUPtr = std::unique_ptr<ParserListener>;

	enum class State {
		InWork,
		Completed,
		Failed,
	};

	ItemsFilterFinder( const String& field );

	std::tuple<bool, DataList> find( common::IReadStream& rStream );

	inline auto parserDepth() const {
		return m_parser.depth();
	}

	inline auto blockDepth() const {
		return m_blockDept;
	}

	inline auto streamPos() const {
		return m_pos;
	}

	template<typename S, typename... Args>
	inline void setState( Args... args );

	void OnFailed( String&& reason );

	void OnCompleted();

	void OnTemplateBlocksArrayWasFounded();

	void OnTemplateStart();

	void OnTemplateEnd();

	void OnItemFilterBlockWasFounded();

	void OnFieldsBlockWasFounded();

	void OnItemBlockWasFounded(const int blockStart, const int blockEnd);

	void OnNamesFieldWasFounded();

	void OnItemFiltersValuesWasReaded( TokensSet&& tokens );

private:
	const String& m_field;
	State m_state = State::InWork;
	int m_blockDept = -1,
		m_pos = 0;
	BlockData m_bData;
	DataList m_dataList;
	Parser m_parser;

	StateUPtr m_state2;
};

class TemplatesGenerator : public JsonListenerBase<ItemsFilterFinder::String> {
public:
	using TokenType = ItemsFilterFinder::TokenType;
	using TokensSet = ItemsFilterFinder::TokensSet;
	using BlockData = ItemsFilterFinder::BlockData;
	using DataList = ItemsFilterFinder::DataList;

	using String = ItemsFilterFinder::String;
	using Parser = JsonStreamingParser<String>;

	using ItemsList = common::block_list<String>;

	TemplatesGenerator( common::JsonStreamGenerator& gen );

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

	bool generate( const ItemsList& itemsList,
			       const DataList& dataList,
				   common::IReadStream& rStream );

private:
	bool checkFilter( const TokenType& token, const TokensSet& filtersSet ) const;

private:
	const BlockData* m_data;
	int m_pos;
	bool m_isSuccess = true;
	String m_currItem;
	String m_key;
	common::JsonStreamGenerator& m_gen;
	Parser m_parser;
};

} // namespace jssp
