
add_library(
	NostalgiaScene
		scene.cpp
)

target_link_libraries(
	NostalgiaScene PUBLIC
		NostalgiaCore
)

install(
	FILES
		scene.hpp
	DESTINATION
		include/nostalgia/scene
)

#if(NOSTALGIA_BUILD_STUDIO)
#	add_subdirectory(studio)
#endif()
