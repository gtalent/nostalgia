add_library(
	OxStd
		assert.cpp
		buildinfo.cpp
		byteswap.cpp
		heapmgr.cpp
		memops.cpp
		new.cpp
		random.cpp
		substitutes.cpp
		stacktrace.cpp
		string.cpp
		strops.cpp
		trace.cpp
)

target_compile_options(OxStd PRIVATE -Wsign-conversion)

if(NOT OX_BARE_METAL)
	set_property(
		TARGET
			OxStd
		PROPERTY
			POSITION_INDEPENDENT_CODE ON
	)
endif()

if(${OX_USE_STDLIB})
	target_compile_definitions(
		OxStd PUBLIC
			OX_USE_STDLIB
	)
endif()

target_link_libraries(
	OxStd PUBLIC
		$<$<CXX_COMPILER_ID:GNU>:gcc>
)

install(
	FILES
		assert.hpp
		bit.hpp
		bstring.hpp
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
		strongint.hpp
		strops.hpp
		std.hpp
		stddef.hpp
		trace.hpp
		types.hpp
		typetraits.hpp
		units.hpp
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
