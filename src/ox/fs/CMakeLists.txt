cmake_minimum_required(VERSION 2.8)

add_library(
	OxFS
		filesystem.cpp
		pathiterator.cpp
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
			oxfstool.cpp
	)
	set_target_properties(oxfstool PROPERTIES OUTPUT_NAME oxfs)
	target_link_libraries(oxfstool OxFS OxStd)
endif()

install(
	FILES
		filestore.hpp
		filesystem.hpp
		pathiterator.hpp
	DESTINATION
		include/ox/fs
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
