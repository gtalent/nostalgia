add_executable(nost-pack pack.cpp)

target_link_libraries(
	nost-pack
		OxClArgs
		NostalgiaPack
)

install(
	TARGETS
		nost-pack
	RUNTIME DESTINATION
		bin
)


add_subdirectory(pack)
