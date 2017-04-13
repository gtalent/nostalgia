cmake_minimum_required(VERSION 2.8.8)

project(nost-pack)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_executable(nost-pack pack.cpp)

target_link_libraries(
	nost-pack
		Qt5::Widgets
		${OxClArgs_LIBRARY}
		${OxFS_LIBRARY}
		${OxStd_LIBRARY}
		NostalgiaCommon
		NostalgiaCore
)

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)

