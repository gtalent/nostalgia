add_executable(
	nostalgia
		main.cpp
)

if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	set(NOSTALGIA_PLAYER_DEPS "NostalgiaCore-GBA")
else()
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
		${NOSTALGIA_PLAYER_DEPS}
)

install(
	TARGETS
		nostalgia
	DESTINATION
		bin
)

