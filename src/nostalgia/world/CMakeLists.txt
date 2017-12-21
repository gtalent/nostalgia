
add_library(
	NostalgiaWorld
		world.cpp
)

#install(TARGETS NostalgiaCommon DESTINATION lib)
install(
	FILES
		world.hpp
	DESTINATION
		include/nostalgia/world
)

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_subdirectory(studio)
endif()
