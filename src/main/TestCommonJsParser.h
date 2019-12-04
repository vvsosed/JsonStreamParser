#pragma once

#include "JSStreamParser.h"

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

void processStreamWithCommonJsParser( common::IReadStream& rStream );

} // namespace jstest
