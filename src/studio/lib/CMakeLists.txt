cmake_minimum_required(VERSION 2.8.11)

project(NostalgiaStudio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio
		newwizard.cpp
		oxfstreeview.cpp
		project.cpp
)

set_property(
	TARGET
		NostalgiaStudio
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
	NostalgiaStudio
		Qt5::Core
		Qt5::Widgets
		OxFS
		OxStd
)

install(
	FILES
		newwizard.hpp
		oxfstreeview.hpp
		project.hpp
	DESTINATION
		include/nostalgia/studio/lib
)
