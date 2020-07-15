if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	enable_language(C ASM)

	add_library(
		NostalgiaCore-GBA
			bios.s
			core.cpp
			gfx.cpp
			irq.arm.cpp
			irq.s
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
			LIBRARY DESTINATION lib/nostalgia
			ARCHIVE DESTINATION lib/nostalgia
	)
endif()

# tests
if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_executable(NostalgiaCore-GBA_Test
		tests.cpp
	)
	target_link_libraries(NostalgiaCore-GBA_Test NostalgiaCore)
endif()
