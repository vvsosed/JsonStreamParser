set(MAIN_SRC
    main.cpp
	TestCommonJsParser.cpp
	TestJssParser.cpp )

add_executable(main ${MAIN_SRC})

target_link_libraries( main common storage jssp)