#pragma once

#include "JsonStreamingParser.h"
#include "JsonListener.h"

#include <list>
#include <set>
#include <tuple>

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

class ItemsFilterFinder : public JsonListener {
public:
	using TokenType = std::string;
	using TokensSet = std::set<TokenType>;
	struct BlockData {
		int m_start = 0, m_end = 0;
		TokensSet m_tokens;

		void reset() {
			m_start = 0;
			m_end = 0;
			m_tokens = TokensSet();
		}
	};

	using DataList = std::list<BlockData>;

	enum class State {
		WaitStartOfTemplatesBlock,
		WaitStartOfBlock,
		WaitItemFilterFieldKey,
		WaitNamesFieldKey,
		WaitArrayBegin,
		ReadItems,
		WaitingEndOfBlock,
		Completed,
		Failed,
	};

	ItemsFilterFinder( const String& field );

	void key(String key) override;

	void value(String value) override;

	void startArray() override;

	void endArray() override;

	void startObject() override;

	void endObject() override;

	std::tuple<bool, DataList> find( common::IReadStream& rStream );

private:
	const String& m_field;
	State m_state = State::WaitStartOfTemplatesBlock;
	int m_blockDept = -1,
		m_pos = 0;
	BlockData m_bData;
	DataList m_dataList;
	JsonStreamingParser m_parser;
};

class TemplatesGenerator : public JsonListener {
public:
	using TokenType = ItemsFilterFinder::TokenType;
	using TokensSet = ItemsFilterFinder::TokensSet;
	using BlockData = ItemsFilterFinder::BlockData;
	using DataList = ItemsFilterFinder::DataList;

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

	bool generate( const std::list<std::string> itemsList,
			       const DataList& dataList,
				   common::IReadStream& rStream );

private:
	bool checkFilter( const TokenType& token, const TokensSet& filtersSet ) const;

	void resetItemBlockFlags();

private:
	bool m_isSuccess = true;
	String m_itemValue;
	String m_key;
	bool m_isItemName = false;
	bool m_isItemType =  false;
	common::JsonStreamGenerator& m_gen;
	JsonStreamingParser m_parser;
};

} // namespace jssp
