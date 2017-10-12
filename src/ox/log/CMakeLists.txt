cmake_minimum_required(VERSION 2.8)

add_library(
	OxLog
		log.cpp
)

set_property(
	TARGET
		OxLog
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		log.hpp
	DESTINATION
		include/ox/mc
)

install(TARGETS OxLog
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)
