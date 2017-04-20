cmake_minimum_required(VERSION 2.8.11)

project(NostalgiaStudio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio
	SHARED
		newwizard.cpp
		project.cpp
)

target_link_libraries(
	NostalgiaStudio
		Qt5::Core
		Qt5::Widgets
		${OxFS_LIBRARY}
		${OxStd_LIBRARY}
)

install(
	FILES
		newwizard.hpp
		project.hpp
	DESTINATION
		include/nostalgia/studio/lib
)
