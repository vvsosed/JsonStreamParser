#pragma once

#include <Mutex.h>
#include <functional>
#include <string>
#include <memory>

namespace storage {

class Storage {
    Storage( common::Mutex::Timeout timeout );

public:
    enum class WalkDir { Continue, Complete };

    using WalkDirClbk = std::function<WalkDir( char* name, char* full )>;

    static Storage& get();

    common::Mutex& getVolumeWrittingMutex();

    bool isExist( const char* filePath );

    bool remove( const char* filePath );

    bool remove( const std::string& filePath ) {
        return remove( filePath.c_str() );
    }

    bool rename( const char* oldFilePath, const char* newFilePath );

    bool walkDir( const char* dirPath, WalkDirClbk onFileClbk, WalkDirClbk onDirClbk );

    bool createDir( const char* dirPath );

    void printDir( const char* dirPath );

private:
    common::Mutex m_mutex;
    common::Mutex m_fatWriteMutex;

    const common::Mutex::Timeout m_timeout;
};

};  // namespace storage
