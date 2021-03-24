set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(
	nostalgia-studio MACOSX_BUNDLE
		json_read.cpp
		json_write.cpp
		main.cpp
		mainwindow.cpp
		oxfstreeview.cpp
		studiorsrc.qrc
)

target_link_libraries(
	nostalgia-studio
		QDarkStyle
		NostalgiaCore-Studio
		NostalgiaStudio
		NostalgiaPack
)

# enable LTO
set_property(TARGET nostalgia-studio PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

if(APPLE)
	set_target_properties(nostalgia-studio PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
endif()

install(
	FILES
		nostalgia-studio.json
	DESTINATION
		${NOSTALGIA_DIST_RESOURCES}
)

install(
	TARGETS
		nostalgia-studio
	RUNTIME DESTINATION
		${NOSTALGIA_DIST_BIN}
	BUNDLE DESTINATION .
)

add_executable(
	NostalgiaStudioJsonTest
		json_read.cpp
		json_write.cpp
		json_test.cpp
)

target_link_libraries(
	NostalgiaStudioJsonTest
		OxStd
		Qt5::Widgets
)

add_test("[nostalgia/studio] NostalgiaStudioJson" NostalgiaStudioJsonTest)

add_subdirectory(lib)
