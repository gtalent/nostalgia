cmake_minimum_required(VERSION 2.8)

add_library(
	OxFS
		filesystem/filesystem.cpp
		filesystem/pathiterator.cpp
)

set_property(
	TARGET
		OxFS
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

if(OX_BUILD_EXEC STREQUAL "ON")
	add_executable(
		oxfstool
			toollib.cpp
			oxfstool.cpp
	)
	set_target_properties(oxfstool PROPERTIES OUTPUT_NAME oxfs)
	target_link_libraries(
		oxfstool
			OxFS
			OxLog
			OxStd
	)
endif()

install(
	FILES
		filestore.hpp
	DESTINATION
		include/ox/fs
)

install(
	FILES
		filesystem/filesystem.hpp
		filesystem/pathiterator.hpp
	DESTINATION
		include/ox/fs/filesystem
)

install(
	TARGETS
		OxFS
	LIBRARY DESTINATION lib/ox
	ARCHIVE DESTINATION lib/ox
)

if(OX_BUILD_EXEC STREQUAL "ON")
	if(OX_RUN_TESTS STREQUAL "ON")
		add_subdirectory(test)
	endif()
	install(TARGETS oxfstool
			  RUNTIME DESTINATION bin
	)
endif()
