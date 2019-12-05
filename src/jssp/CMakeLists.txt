set( STORAGE_SRC
     src/JsonListener.cpp
	 src/JsonStreamingParser.cpp )

add_library( jssp STATIC ${STORAGE_SRC})

target_include_directories( jssp PUBLIC include )

target_link_libraries( jssp common )
