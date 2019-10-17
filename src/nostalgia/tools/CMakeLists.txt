set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_executable(nost-pack pack.cpp)

target_link_libraries(
	nost-pack
		NostalgiaPack
)

install(
	TARGETS
		nost-pack
	RUNTIME DESTINATION
		bin
)


add_subdirectory(pack)
