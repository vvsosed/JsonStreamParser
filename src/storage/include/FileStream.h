#pragma once

#include "IReadStream.h"
#include "IWriteStream.h"
#include "StorageFile.h"

namespace storage {

class WriteFileStream : common::IWriteStream {
public:
    using UPtr = common::IWriteStream::UPtr;

    static UPtr open( const char* filename, const char* mode );

    WriteFileStream() = default;

    WriteFileStream( File::UPtr&& ptr );

    std::size_t write( const char* buffer, std::size_t size ) override;

    bool flush() override;

    bool close() override;

private:
    File::UPtr m_file;
};

class ReadFileStream : public common::IReadStream {
public:
    using UPtr = std::unique_ptr<ReadFileStream>;

    static UPtr open( const char* filename );

    ReadFileStream() = default;

    ReadFileStream( File::UPtr&& ptr );

    size_t read( char* buff, size_t bufSize ) override;

    size_t size() const override;

    bool reset(unsigned int offset = 0) override;

    auto& file() {
    	return m_file;
    }

private:
    File::UPtr m_file;
};

}  // namespace storage
