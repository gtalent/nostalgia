cmake_minimum_required(VERSION 2.8.11)

project(NostalgiaStudio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio
	SHARED
		newwizard.cpp
)

target_link_libraries(
	NostalgiaStudio
		Qt5::Core
		Qt5::Widgets
)

install(
	FILES
		newwizard.hpp
	DESTINATION
		include/nostalgia/studio/lib
)
