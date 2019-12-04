#include <iostream>

#include "IWriteStream.h"
#include "StorageFile.h"
#include "FileStream.h"

#include "TestCommonJsParser.h"

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

int main(int argc, char**argv) {
	std::cout << "We need json streaming parser!" << std::endl;

	//storage::File file1, file2;
	//openFileForReading(file1, JS_FILE1);
	//openFileForReading(file2, JS_FILE2);

	//auto fStream1 = createReadFileStream(JS_FILE1);
	auto fStream2 = createReadFileStream(JS_FILE2);

	//outputStream(*fStream1);
	//fStream1->reset();
	//jstest::processStreamWithCommonJsParser(*fStream1);

	//outputStream(*fStream2);
	fStream2->reset();
	//jstest::processStreamWithCommonJsParser(*fStream2);
	jstest::filteringItemsWithCommonJsParser(*fStream2);

   return 0; 
}
