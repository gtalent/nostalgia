cmake_minimum_required(VERSION 2.8.11)

project(nostalgia)

add_executable(
	nostalgia
		main.cpp
)

if(COMMAND OBJCOPY_FILE)
	set_target_properties(nostalgia
		PROPERTIES
		LINK_FLAGS ${LINKER_FLAGS}
		COMPILER_FLAGS "-mthumb -mthumb-interwork"
	)

	OBJCOPY_FILE(nostalgia)
	#PADBIN_FILE(nostalgia)
endif()

target_link_libraries(
	nostalgia
		NostalgiaCore
		OxStd
		OxFS
)

add_custom_target("nostalgia.bin")

install(
	TARGETS
		nostalgia
	DESTINATION
		bin
)

if (NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	install(
		FILES
			nostalgia.bin
		DESTINATION
			bin
	)
endif()

