#pragma once

#include "JSStreamParser.h"

#include <list>
#include <set>

namespace common {
class IReadStream;
class JsonStreamGenerator;
} // namespace common

namespace jstest {

class TestCommonJsParser : public common::JSStreamParser {
public:
	TestCommonJsParser( const int buffSize = 1024 );

	~TestCommonJsParser();

	bool processActiveToken() override;

	bool onObjectBegin() override;

	bool onObjectEnd() override;

	bool onArrayBegin() override;

	bool onArrayEnd() override;

	bool onLongToken() override;

	bool onNext() override;
};

class ItemFilterParser : public TestCommonJsParser {
public:
	using TokenType = std::string;
	using TokensSet = std::set<TokenType>;
	using DataList = std::list<TokensSet>;

	enum State {
		WaitItemFilterFieldKey,
		WaitNamesFieldKey,
		WaitArrayBegin,
		ReadItems
	};

	using TestCommonJsParser::TestCommonJsParser;

	bool processActiveToken() override;

	bool onObjectBegin() override;

	bool onObjectEnd() override;

	bool onArrayBegin() override;

	bool onArrayEnd() override;

	bool onLongToken() override;

	bool onNext() override;

	void printResult();

	DataList popDataList() {
		return std::move(m_dataList);
	}

private:
	State m_state = WaitItemFilterFieldKey;
	TokensSet m_currentSet;
	DataList m_dataList;
};

class ScenesTemplatesGenerator : public TestCommonJsParser {
public:
	using TokenType = ItemFilterParser::TokenType;
	using TokensSet = ItemFilterParser::TokensSet;
	using DataList = ItemFilterParser::DataList;

	enum class State {
		FindingBeginOfTemplate,
		FindingFieldsKey,
		FindingFieldObject,
		FindingBlock,
		FindingValue,
		FindingEndOfTemplate
	};

	ScenesTemplatesGenerator( common::JsonStreamGenerator& gen, const DataList& dataList );

	bool processActiveToken() override;

	bool onObjectBegin() override;

	bool onObjectEnd() override;

	bool onArrayBegin() override;

	bool onArrayEnd() override;

	bool onLongToken() override;

	bool onNext() override;

private:
	State m_state = State::FindingBeginOfTemplate;
	common::JsonStreamGenerator& m_gen;
	const DataList& m_dataList;
};

void processStreamWithCommonJsParser( common::IReadStream& rStream );

ItemFilterParser::DataList filteringItemsWithCommonJsParser( common::IReadStream& rStream );

} // namespace jstest
