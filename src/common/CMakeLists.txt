add_library(common STATIC
	src/BlocksAllocator.cpp
	src/Buffer.cpp
	src/CommonResources.cpp
	src/JSStreamParser.cpp
	src/IValueVisitor.cpp
	src/IValue.cpp
	src/IValueCast.cpp
	src/JsonStreamGenerator.cpp

	src/values/BoolValue.cpp 
	src/values/FloatValue.cpp
	src/values/ScalebleValue.cpp
	src/values/IntValue.cpp
	src/values/NotificationValue.cpp
	src/values/RgbValue.cpp
	src/values/StringValue.cpp
	src/values/ArrayValue.cpp
	src/values/TableValue.cpp
	src/values/UserCodeValue.cpp
	)

target_include_directories(common PUBLIC include )

#target_link_libraries(common hub_core network znet freertos esp32 soc heap newlib log nvs_flash spi_flash )
