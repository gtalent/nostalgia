
add_library(
	NostalgiaWorld
		world.cpp
)

target_link_libraries(
	NostalgiaWorld PUBLIC
		NostalgiaCore
		OxMetalClaw
)

#install(TARGETS NostalgiaCommon DESTINATION lib)
install(
	FILES
		world.hpp
	DESTINATION
		include/nostalgia/world
)

if(NOSTALGIA_BUILD_STUDIO)
	add_subdirectory(studio)
endif()
