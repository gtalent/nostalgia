cmake_minimum_required(VERSION 2.8)

add_library(
	OxStd
		memops.cpp
		random.cpp
		strops.cpp
)

set_property(
	TARGET
		OxStd
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		bitops.hpp
		byteswap.hpp
		memops.hpp
		random.hpp
		string.hpp
		strops.hpp
		std.hpp
		types.hpp
	DESTINATION
		include/ox/std
)

install(TARGETS OxStd
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)

if(OX_RUN_TESTS STREQUAL "ON")
	add_subdirectory(test)
endif()
