cmake_minimum_required(VERSION 2.8)

add_library(
	OxStd
		assert.cpp
		byteswap.cpp
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
		assert.hpp
		bitops.hpp
		byteswap.hpp
		math.hpp
		memops.hpp
		new.hpp
		random.hpp
		string.hpp
		strops.hpp
		std.hpp
		stddef.hpp
		types.hpp
		typetraits.hpp
		vector.hpp
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
