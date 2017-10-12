
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
