cmake_minimum_required(VERSION 2.8.11)

project(NostalgiaStudio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudioStatic
		newwizard.cpp
		project.cpp
)

set_property(
	TARGET
		NostalgiaStudioStatic
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
	NostalgiaStudioStatic
		Qt5::Core
		Qt5::Widgets
		${OxFS_LIBRARY}
		${OxStd_LIBRARY}
)

add_library(
	NostalgiaStudio
	SHARED
)

target_link_libraries(
	NostalgiaStudio
		Qt5::Core
		Qt5::Widgets
		NostalgiaStudioStatic
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
