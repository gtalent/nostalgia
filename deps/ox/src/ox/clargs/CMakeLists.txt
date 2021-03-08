cmake_minimum_required(VERSION 3.10)

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

target_link_libraries(
	OxClArgs PUBLIC
		OxStd
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
