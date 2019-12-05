#include "FileStream.h"

namespace storage {

WriteFileStream::UPtr WriteFileStream::open( const char* filename, const char* mode ) {
    auto file = std::make_unique<File>();
    if( !file || !file->open( filename, mode ) ) {
        return nullptr;
    }

    return UPtr( new WriteFileStream( std::move( file ) ) );
}

WriteFileStream::WriteFileStream( File::UPtr&& ptr )
: m_file( std::move( ptr ) ) {}

std::size_t WriteFileStream::write( const char* buffer, std::size_t size ) {
    return m_file && m_file->write( buffer, size ) ? size : 0;
}

bool WriteFileStream::flush() {
    return m_file && m_file->seek( 0, SEEK_SET );
}

bool WriteFileStream::close() {
    if( !m_file ) {
        return false;
    }
    m_file->close();
    return true;
}

ReadFileStream::UPtr ReadFileStream::open( const char* filename ) {
    auto file = std::make_unique<File>();
    if( !file || !file->open( filename, "r" ) ) {
        return nullptr;
    }
    auto stream = UPtr( new ReadFileStream( std::move( file ) ) );
    return stream;
}

ReadFileStream::ReadFileStream( File::UPtr&& ptr )
: m_file( std::move( ptr ) ) {}

size_t ReadFileStream::read( char* buff, size_t bufSize ) {
    size_t readed;
    return m_file && m_file->read( buff, bufSize, &readed ) ? readed : 0;
}

size_t ReadFileStream::size() const {
    return m_file ? 1 : 0;
}

bool ReadFileStream::reset(unsigned int offset) {
	return m_file ? m_file->seek(offset, SEEK_SET) : false;
}

}  // namespace storage
