cmake_minimum_required(VERSION 2.8.11)

project(NostalgiaStudioJson)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudioJson
		json_read.cpp
		json_write.cpp
)

target_link_libraries(
	NostalgiaStudioJson
		Qt5::Core
		Qt5::Widgets
)

install(
	FILES
		json.hpp
		json_read.hpp
		json_write.hpp
	DESTINATION
		include/nostalgia/studio/json
)


add_executable(
	NostalgiaStudioJsonTest
		test.cpp
)

target_link_libraries(
	NostalgiaStudioJsonTest
		NostalgiaStudioJson
)

add_test("Test\\ NostalgiaStudioJson" NostalgiaStudioJsonTest)
