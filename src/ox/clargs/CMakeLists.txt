cmake_minimum_required(VERSION 2.8)

add_library(
	OxClArgs
		clargs.cpp
)

set_property(
	TARGET
		OxClArgs
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		clargs.hpp
	DESTINATION
		include/ox/clargs
)

install(
	TARGETS
		OxClArgs
	LIBRARY DESTINATION lib/ox
	ARCHIVE DESTINATION lib/ox
)
