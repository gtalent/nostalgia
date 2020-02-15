set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio SHARED
		editor.cpp
		json_read.cpp
		json_write.cpp
		wizard.cpp
		plugin.cpp
		project.cpp
)

install(TARGETS NostalgiaStudio
        LIBRARY DESTINATION ${NOSTALGIA_DIST_LIB}/nostalgia)

generate_export_header(NostalgiaStudio)
target_include_directories(NostalgiaStudio PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(
	NostalgiaStudio PUBLIC
		Qt5::Widgets
		OxFS
		OxMetalClaw
)

install(
	FILES
		editor.hpp
		json.hpp
		json_err.hpp
		json_read.hpp
		json_write.hpp
		wizard.hpp
		plugin.hpp
		project.hpp
		${PROJECT_BINARY_DIR}/nostalgiastudio_export.h
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
