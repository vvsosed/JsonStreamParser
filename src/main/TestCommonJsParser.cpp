#include "TestCommonJsParser.h"
#include "BlocksAllocator.h"
#include "IReadStream.h"
#include "JsonStreamGenerator.h"

#include <iostream>

namespace jstest {

TestCommonJsParser::TestCommonJsParser( const int buffSize )
: common::JSStreamParser(static_cast<char*>(common::blocks_allocator::allocate(buffSize)), buffSize) {}

TestCommonJsParser::~TestCommonJsParser() {
	common::blocks_allocator::deallocate(getBuffer());
}

bool TestCommonJsParser::processActiveToken() {
	std::cout << "processActiveToken: " << getTokenValue() << std::endl;
	return true;
}

bool TestCommonJsParser::onObjectBegin() {
	std::cout << "onObjectBegin dept=" << depth() << std::endl;
	return true;
}

bool TestCommonJsParser::onObjectEnd() {
	std::cout << "onObjectEnd" << std::endl;
	return true;
}

bool TestCommonJsParser::onArrayBegin() {
	std::cout << "onArrayBegin dept=" << depth() << std::endl;
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


bool ItemFilterParser::processActiveToken() {
	std::string value = getTokenValue();
	if (WaitItemFilterFieldKey == m_state) {
		if ( value == "item_filter") {
			m_state = WaitNamesFieldKey;
		}
	}
	else if (WaitNamesFieldKey == m_state) {
		if ( value == "names") {
			m_state = WaitArrayBegin;
		}
	}
	else if (m_state == ReadItems) {
		if ( !isQuotedTokenProcessing() ) {
			return false;
		}
		m_currentSet.emplace(std::move(value));
	}
	return true;
}

bool ItemFilterParser::onObjectBegin() {
	return m_state != ReadItems;
}

bool ItemFilterParser::onObjectEnd() {
	return m_state != ReadItems;
}

bool ItemFilterParser::onArrayBegin() {
	if (m_state == WaitArrayBegin) {
		m_state = ReadItems;
		return true;
	}
	return m_state != ReadItems;
}

bool ItemFilterParser::onArrayEnd() {
	if (m_state == ReadItems) {
		m_state = WaitItemFilterFieldKey;
		m_dataList.emplace_back(std::move(m_currentSet));
		m_currentSet = TokensSet();
	}
	return true;
}

bool ItemFilterParser::onLongToken() {
	return true;
}

bool ItemFilterParser::onNext() {
	return true;
}

void ItemFilterParser::printResult() {
	int counter = 0;
	for(const auto& itemsSet : m_dataList) {
		std::cout << "template " << ++counter << std::endl;
		for (const auto& item : itemsSet) {
			std::cout << item << ",";
		}
		std::cout << "\n-------------------------------" << std::endl;
	}
}

ScenesTemplatesGenerator::ScenesTemplatesGenerator( common::JsonStreamGenerator& gen, const DataList& dataList )
: m_gen(gen), m_dataList(dataList) {}

bool ScenesTemplatesGenerator::processActiveToken() {
	return true;
}

bool ScenesTemplatesGenerator::onObjectBegin() {
	return true;
}

bool ScenesTemplatesGenerator::onObjectEnd() {
	return true;
}

bool ScenesTemplatesGenerator::onArrayBegin() {
	return true;
}

bool ScenesTemplatesGenerator::onArrayEnd() {
	return true;
}

bool ScenesTemplatesGenerator::onLongToken() {
	return true;
}

bool ScenesTemplatesGenerator::onNext() {
	return true;
}

void processStreamWithCommonJsParser( common::IReadStream& rStream ) {
	std::cout << "\n---------- Run processStreamWithCommonJsParser ..." << std::endl;
	TestCommonJsParser testParser;
	auto isSuccess = testParser.parse(rStream);
	std::cout << "---------- Complete processStreamWithCommonJsParser: " << (isSuccess ? "ok" : "fail") << ".\n" << std::endl;
}

ItemFilterParser::DataList filteringItemsWithCommonJsParser( common::IReadStream& rStream ) {
	ItemFilterParser itemsParser;
	auto isSuccess = itemsParser.parse(rStream);
	if (isSuccess) {
		itemsParser.printResult();
	}
	std::cout << "---------- Complete filteringItemsWithCommonJsParser: " << (isSuccess ? "ok" : "fail") << ".\n" << std::endl;
	return isSuccess ? itemsParser.popDataList() : ItemFilterParser::DataList();
}

} // namespace jstest

