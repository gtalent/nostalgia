add_library(
	OxStd
		arith.cpp
		assert.cpp
		buildinfo.cpp
		byteswap.cpp
		memops.cpp
		new.cpp
		random.cpp
		stacktrace.cpp
		strops.cpp
		trace.cpp
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
		buildinfo.hpp
		byteswap.hpp
		defines.hpp
		error.hpp
		hardware.hpp
		hashmap.hpp
		math.hpp
		memops.hpp
		new.hpp
		random.hpp
		stacktrace.hpp
		string.hpp
		strops.hpp
		std.hpp
		stddef.hpp
		trace.hpp
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
