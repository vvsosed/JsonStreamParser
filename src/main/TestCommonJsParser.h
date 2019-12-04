#pragma once

#include "JSStreamParser.h"

#include <list>
#include <set>

namespace common {
class IReadStream;
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

private:
	State m_state = WaitItemFilterFieldKey;
	TokensSet m_currentSet;
	DataList m_dataList;
};

void processStreamWithCommonJsParser( common::IReadStream& rStream );

void filteringItemsWithCommonJsParser( common::IReadStream& rStream );

} // namespace jstest
