set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaStudio
		editor.cpp
		wizard.cpp
		module.cpp
		project.cpp
)

install(TARGETS NostalgiaStudio
        LIBRARY DESTINATION ${NOSTALGIA_DIST_LIB}/nostalgia)

generate_export_header(NostalgiaStudio)
target_include_directories(NostalgiaStudio PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(
	NostalgiaStudio PUBLIC
		Qt${QT_VERSION_MAJOR}::Widgets
		OxFS
		OxClaw
)

install(
	FILES
		editor.hpp
		wizard.hpp
		module.hpp
		project.hpp
		${CMAKE_CURRENT_BINARY_DIR}/nostalgiastudio_export.h
	DESTINATION
		include/nostalgia/studio/lib
)
