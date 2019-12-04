set(STORAGE_SRC
    src/Storage.cpp
	src/StorageFile.cpp
	src/FileStream.cpp
	src/Volume.cpp
	src/crc16.c )

add_library(storage STATIC ${STORAGE_SRC})

target_include_directories(storage PUBLIC include )

target_link_libraries(storage common)

