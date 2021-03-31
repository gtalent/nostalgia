add_executable(
	nostalgia
		app.cpp
		main.cpp
)

# enable LTO
set_property(TARGET nostalgia PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

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
		$<$<STREQUAL:${NOSTALGIA_BUILD_TYPE},GBA>:NostalgiaCore-GBA>
		$<$<STREQUAL:${NOSTALGIA_BUILD_TYPE},Native>:NostalgiaCore-SDL>
)

install(
	TARGETS
		nostalgia
	DESTINATION
		bin
)

