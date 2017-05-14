cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaTool
		tilesetimport.cpp
)

target_link_libraries(
	NostalgiaTool
		Qt5::Widgets
		OxClArgs
		OxFS
		OxStd
		NostalgiaCommon
		NostalgiaCore
)

install(
	FILES
		tilesetimport.hpp
	DESTINATION
		include/nostalgia/tools/lib
)

install(
	TARGETS
		NostalgiaTool
	LIBRARY DESTINATION lib/ox
	ARCHIVE DESTINATION lib/ox
)
