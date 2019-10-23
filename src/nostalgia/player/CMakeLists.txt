
if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	enable_language(C ASM)
	add_executable(
		nostalgia
			main.cpp
			startup.s
	)
	set(NOSTALGIA_PLAYER_DEPS "")
else()
	add_executable(
		nostalgia
			main.cpp
	)
	set(NOSTALGIA_PLAYER_DEPS "NostalgiaCore-SDL")
endif()

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
		NostalgiaWorld
		NostalgiaCore
		${NOSTALGIA_PLAYER_DEPS}
)

add_custom_target("nostalgia.bin")

install(
	TARGETS
		nostalgia
	DESTINATION
		bin
)

