if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	add_library(
		NostalgiaCore-GBA
			core.cpp
			gfx.cpp
			irq.arm.cpp
			irq.cpp
			media.cpp
			panic.cpp
	)

	set_source_files_properties(irq.arm.cpp PROPERTIES COMPILE_FLAGS -marm)

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
	)
	target_link_libraries(NostalgiaCore-GBA_Test NostalgiaCore)
endif()
