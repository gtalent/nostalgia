
find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(
	QDarkStyle OBJECT
		theme.cpp
		qdarkstyle/style.qrc
)

target_include_directories(
	QDarkStyle PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

target_link_libraries(
	QDarkStyle
		Qt5::Widgets
)

