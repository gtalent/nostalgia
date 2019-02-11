cmake_minimum_required(VERSION 2.8)

add_library(
	OxMetalClaw
		defwriter.cpp
		presencemask.cpp
		read.cpp
		write.cpp
)

target_link_libraries(
	OxMetalClaw PUBLIC
		OxStd
		OxTrace
)

set_property(
	TARGET
		OxMetalClaw
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		defwriter.hpp
		err.hpp
		presencemask.hpp
		read.hpp
		write.hpp
	DESTINATION
		include/ox/mc
)

install(TARGETS OxMetalClaw
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)

if(OX_RUN_TESTS STREQUAL "ON")
	add_subdirectory(test)
endif()
