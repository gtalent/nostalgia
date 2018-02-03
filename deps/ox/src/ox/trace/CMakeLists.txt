cmake_minimum_required(VERSION 2.8)

add_library(
	OxTrace
		trace.cpp
)

set_property(
	TARGET
		OxTrace
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		trace.hpp
	DESTINATION
		include/ox/mc
)

install(TARGETS OxTrace
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)
