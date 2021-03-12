if(OX_USE_STDLIB AND OX_ENABLE_TRACEHOOK)
	add_library(
		OxTraceHook SHARED
			tracehook.cpp
	)
else()
	add_library(
		OxTraceHook
			tracehook.cpp
	)
endif()

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

target_compile_definitions(
	OxStd PUBLIC
		$<$<BOOL:${OX_USE_STDLIB}>:OX_USE_STDLIB>
		$<$<BOOL:${OX_NODEBUG}>:OX_NODEBUG>
)

target_link_libraries(
	OxStd PUBLIC
		$<$<CXX_COMPILER_ID:GNU>:gcc>
		OxTraceHook
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
		fmt.hpp
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

install(TARGETS OxStd OxTraceHook
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)

if(OX_RUN_TESTS)
	add_subdirectory(test)
endif()
