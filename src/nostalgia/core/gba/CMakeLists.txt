if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	add_library(
		NostalgiaCore-GBA
			core.cpp
			gfx.cpp
			media.cpp
			mem.cpp
			panic.cpp
	)

	target_link_libraries(
		NostalgiaCore-GBA PUBLIC
			NostalgiaCore
			GbaStartup
			OxFS
			OxStd
	)

	install(
		TARGETS
			NostalgiaCore-GBA
		DESTINATION
			include/nostalgia/core
	)
endif()

# tests
if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_executable(NostalgiaCore-GBA_Test
		tests.cpp
		mem.cpp
	)
	target_link_libraries(NostalgiaCore-GBA_Test NostalgiaCore)
	add_test("NostalgiaCore-GBA\\ Test\\ malloc" NostalgiaCore-GBA_Test malloc)
endif()