#include "TestCommonJsParser.h"
#include "BlocksAllocator.h"
#include "IReadStream.h"

#include <iostream>

namespace jstest {

TestCommonJsParser::TestCommonJsParser( const int buffSize )
: common::JSStreamParser(static_cast<char*>(common::blocks_allocator::allocate(buffSize)), buffSize) {
	std::cout << "Create TestCommonJsParser instance..." << std::endl;
}

TestCommonJsParser::~TestCommonJsParser() {
	common::blocks_allocator::deallocate(getBuffer());
	std::cout << "Destroy TestCommonJsParser instance." << std::endl;
}

bool TestCommonJsParser::processActiveToken() {
	std::cout << "processActiveToken: " << getTokenValue() << std::endl;
	return true;
}

bool TestCommonJsParser::onObjectBegin() {
	std::cout << "onObjectBegin" << std::endl;
	return true;
}

bool TestCommonJsParser::onObjectEnd() {
	std::cout << "onObjectEnd" << std::endl;
	return true;
}

bool TestCommonJsParser::onArrayBegin() {
	std::cout << "onArrayBegin" << std::endl;
	return true;
}

bool TestCommonJsParser::onArrayEnd() {
	std::cout << "onArrayEnd" << std::endl;
	return true;
}

bool TestCommonJsParser::onLongToken() {
	std::cout << "onLongToken" << std::endl;
	return true;
}

bool TestCommonJsParser::onNext() {
	std::cout << "onNext" << std::endl;
	return true;
}

void processStreamWithCommonJsParser( common::IReadStream& rStream ) {
	std::cout << "\n---------- Run processStreamWithCommonJsParser ..." << std::endl;
	TestCommonJsParser testParser;
	auto isSuccess = testParser.parse(rStream);
	std::cout << "---------- Complete processStreamWithCommonJsParser: " << (isSuccess ? "ok" : "fail") << ".\n" << std::endl;
}

} // namespace jstest

