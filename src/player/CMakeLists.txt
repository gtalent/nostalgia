cmake_minimum_required(VERSION 2.8.8)

project(nostalgia)

set(WOMBAT_BUILD_TYPE "Native" CACHE STRING "The type of build to produce(Native/GBA)")

add_executable(
	nostalgia
		main.cpp
)

if(COMMAND objcopy_file)
	set_target_properties(nostalgia
		PROPERTIES
		LINK_FLAGS ${LINKER_FLAGS}
		COMPILER_FLAGS "-mthumb -mthumb-interwork"
	)

	objcopy_file(nostalgia)
endif()

target_link_libraries(nostalgia NostalgiaCore)
