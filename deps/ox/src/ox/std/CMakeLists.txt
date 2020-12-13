add_library(
	OxStd
		assert.cpp
		buildinfo.cpp
		byteswap.cpp
		heapmgr.cpp
		memops.cpp
		random.cpp
		substitutes.cpp
		stacktrace.cpp
		string.cpp
		strops.cpp
		trace.cpp
)

if(NOT MSVC)
	target_compile_options(OxStd PRIVATE -Wsign-conversion)
endif()

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
		heapmgr.hpp
		math.hpp
		memops.hpp
		new.hpp
		random.hpp
		std.hpp
		stddef.hpp
		stacktrace.hpp
		string.hpp
		strongint.hpp
		strops.hpp
		trace.hpp
		typeinfo.hpp
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
