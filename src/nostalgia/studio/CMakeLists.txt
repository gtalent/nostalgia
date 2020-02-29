set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
	nostalgia-studio MACOSX_BUNDLE
		json_read.cpp
		json_write.cpp
		main.cpp
		mainwindow.cpp
		oxfstreeview.cpp
)

target_link_libraries(
	nostalgia-studio
		NostalgiaStudio
		NostalgiaPack
)

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

add_test("Test\\ NostalgiaStudioJson" NostalgiaStudioJsonTest)

add_subdirectory(lib)
