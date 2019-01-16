cmake_minimum_required(VERSION 2.8.11)

project(nost-pack)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_executable(nost-pack pack.cpp)

target_link_libraries(
	nost-pack
		Qt5::Widgets
		OxClArgs
		OxFS
		OxStd
		OxTrace
		OxMetalClaw
		NostalgiaCommon
		NostalgiaCore
)

install(
	TARGETS
		nost-pack
	RUNTIME DESTINATION
		bin
)

add_subdirectory(lib)
