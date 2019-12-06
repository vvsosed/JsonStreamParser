#include <iostream>

#include "IWriteStream.h"
#include "StorageFile.h"
#include "FileStream.h"
#include "JsonStreamGenerator.h"

#include "TestCommonJsParser.h"
#include "TestJssParser.h"

const char JS_FILE1[] = "/home/volodymyr/Documents/repo/JsonStreamParser/src/data/config.json";
const char JS_FILE2[] = "/home/volodymyr/Documents/repo/JsonStreamParser/src/data/scene_blocks.json";

inline bool openFileForReading( storage::File& file, const char* fname ) {
	auto isSuccess = file.open(fname, "r");
	std::cout << "File " << fname << " opened: " << (isSuccess ? "ok." : "fail.") << std::endl;
	return isSuccess;
}

inline auto createReadFileStream( const char* fname ) {
	auto rStream = storage::ReadFileStream::open(fname);
	std::cout << "ReadFileStream for file " << fname << " opened: " << (rStream ? "ok." : "fail.") << std::endl;
	return std::move(rStream);
}

void outputStream( common::IReadStream& stream ) {
	std::cout << "\n-------------------- Begin output stream" << std::endl;
	char ch;
	while( stream.read(&ch, 1) ) {
		std::cout << ch;
	}
	std::cout << "\n-------------------- Finish output stream" << std::endl;
}

void test1() {
	storage::File file1, file2;
	openFileForReading(file1, JS_FILE1);
	openFileForReading(file2, JS_FILE2);
}

void test2() {
	auto fStream1 = createReadFileStream(JS_FILE1);
	auto fStream2 = createReadFileStream(JS_FILE2);

	outputStream(*fStream1);
	fStream1->reset();
	jstest::processStreamWithCommonJsParser(*fStream1);

	outputStream(*fStream2);
	fStream2->reset();
	jstest::processStreamWithCommonJsParser(*fStream2);
}

class ConsoleOutputStream : public common::IWriteStream {
public:
	std::size_t write( const char* buffer, std::size_t size ) override {
		auto count = size;
		while( count-- ) {
			std::cout << *(buffer++);
		}
		return size;
	}

	bool flush() override {
		return true;
	}

	bool close() override {
		return true;
	}
};

void test3() {
	ConsoleOutputStream ostream;
	common::JsonStreamGenerator jsGen(ostream);

	jsGen.writeOpeningObject();
	jsGen.writeProperty("hello", "world");
	jsGen.writeClosingSymbol();
	jsGen.writeJsonEnd();
}

void test4() {
	auto fStream2 = createReadFileStream(JS_FILE2);
	auto dataList = jstest::filteringItemsWithCommonJsParser(*fStream2);
	fStream2->reset();

	ConsoleOutputStream ostream;
	common::JsonStreamGenerator jsGen(ostream);

	jstest::ScenesTemplatesGenerator tmplGen(jsGen, dataList);
}

void testJssp1() {
	auto fStream1 = createReadFileStream(JS_FILE1);
	jssp::testJsonPrinter(*fStream1);
	auto fStream2 = createReadFileStream(JS_FILE2);
	jssp::testJsonPrinter(*fStream2);
}

void testJssp2() {
	auto fStream = createReadFileStream(JS_FILE2);
	jssp::ItemsFilterFinder finder("then");
	jssp::ItemsFilterFinder::DataList dList;
	bool isSuccess;
	std::tie(isSuccess, dList) = finder.find(*fStream);
	for (const auto& data : dList) {
		std::cout << "start=" << data.m_start
				  << "  end=" << data.m_end
				  << "  items=" << data.m_tokens.size()
				  << std::endl;

		if ( !fStream->reset(data.m_start) ) {
			std::cerr << "Can't reset stream!" << std::endl;
			return;
		}

		auto bytesCount = data.m_end - data.m_start;
		while( bytesCount-- ) {
			char ch;
			if ( !fStream->read(&ch, 1) ) {
				std::cout << "\nCan't read next char from stream!" << std::endl;
				return;
			}
			std::cout << ch;
		}
		std::cout << std::endl;
	}
}

void testJssp3() {
	auto fStream = createReadFileStream(JS_FILE2);
	jssp::ItemsFilterFinder finder("then");
	jssp::ItemsFilterFinder::DataList dList;
	bool isSuccess;
	std::tie(isSuccess, dList) = finder.find(*fStream);
	for (const auto& data : dList) {
		std::cout << "start=" << data.m_start
				  << "  end=" << data.m_end
				  << "  items=" << data.m_tokens.size()
				  << std::endl;

		if ( !fStream->reset(data.m_itemBlockStart) ) {
			std::cerr << "Can't reset stream!" << std::endl;
			return;
		}

		auto bytesCount = data.m_itemBlockEnd - data.m_itemBlockStart;
		while( bytesCount-- ) {
			char ch;
			if ( !fStream->read(&ch, 1) ) {
				std::cout << "\nCan't read next char from stream!" << std::endl;
				return;
			}
			std::cout << ch;
		}
		std::cout << std::endl;
	}
}

void testJssp4() {
	auto fStream = createReadFileStream(JS_FILE2);
	jssp::ItemsFilterFinder finder("then");
	jssp::ItemsFilterFinder::DataList dataList;
	bool isSuccess;
	std::tie(isSuccess, dataList) = finder.find(*fStream);
	if ( !isSuccess ) {
		std::cerr << "Can't find filter items" << std::endl;
		return;
	}

	ConsoleOutputStream ostream;
	common::JsonStreamGenerator jsGen(ostream);
	jsGen.writePropertyKey("result") && jsGen.writeOpeningArray();

	jssp::TemplatesGenerator::ItemsList itemsList({
		{"dimmer1"},
		{"dimmer2"},
		{"dimmer3"},
		{"dimmer4"},
		{"meter1"},
		{"meter2"},
	});

	jssp::TemplatesGenerator tmplGen(jsGen);
	if ( !tmplGen.generate(itemsList, dataList, *fStream) ) {
		std::cerr << "Can't generate json from template properly" << std::endl;
	}
	else {
		jsGen.writeClosingSymbol();
		jsGen.~JsonStreamGenerator();
		std::cout << "\nGenerated json successfully!" << std::endl;
	}
}

int main(int argc, char**argv) {
	std::cout << "We need json streaming parser!" << std::endl;

	testJssp4();

	return 0;
}
