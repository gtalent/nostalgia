cmake_minimum_required(VERSION 2.8.11)

project(NostalgiaStudio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio SHARED
		json_read.cpp
		json_write.cpp
		wizard.cpp
		oxfstreeview.cpp
		plugin.cpp
		project.cpp
)

install(TARGETS NostalgiaStudio
        LIBRARY DESTINATION ${NOSTALGIA_DIST_LIB}/nostalgia)

target_link_libraries(
	NostalgiaStudio PUBLIC
		Qt5::Widgets
		OxFS
		OxMetalClaw
)

install(
	FILES
		json.hpp
		json_err.hpp
		json_read.hpp
		json_write.hpp
		wizard.hpp
		oxfstreeview.hpp
		plugin.hpp
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
