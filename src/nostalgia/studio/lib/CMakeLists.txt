set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio SHARED
		editor.cpp
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
		wizard.hpp
		plugin.hpp
		project.hpp
		${CMAKE_CURRENT_BINARY_DIR}/nostalgiastudio_export.h
	DESTINATION
		include/nostalgia/studio/lib
)
