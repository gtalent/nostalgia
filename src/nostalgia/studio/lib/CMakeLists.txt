cmake_minimum_required(VERSION 2.8.11)

project(NostalgiaStudio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio
		json_read.cpp
		json_write.cpp
		wizard.cpp
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
		json.hpp
		json_read.hpp
		json_write.hpp
		newwizard.hpp
		oxfstreeview.hpp
		project.hpp
	DESTINATION
		include/nostalgia/studio/lib
)

add_executable(
	NostalgiaStudioJsonTest
		json_test.cpp
)

target_link_libraries(
	NostalgiaStudioJsonTest
		NostalgiaStudio
)

add_test("Test\\ NostalgiaStudioJson" NostalgiaStudioJsonTest)
