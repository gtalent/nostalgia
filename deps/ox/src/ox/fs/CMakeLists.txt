
add_library(
	OxFS
		filestore/filestoretemplate.cpp
		filesystem/filelocation.cpp
		filesystem/pathiterator.cpp
		filesystem/directory.cpp
		filesystem/filesystem.cpp
		filesystem/passthroughfs.cpp
)

target_link_libraries(
	OxFS PUBLIC
		c++fs
		OxMetalClaw
		OxStd
		OxTrace
)

set_property(
	TARGET
		OxFS
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

if(OX_BUILD_EXEC STREQUAL "ON")
	#add_executable(
	#	oxfstool
	#		toollib.cpp
	#		oxfstool.cpp
	#)
	#set_target_properties(oxfstool PROPERTIES OUTPUT_NAME oxfs)
	#target_link_libraries(
	#	oxfstool
	#		OxFS
	#		OxTrace
	#		OxMetalClaw
	#		OxStd
	#)
endif()

install(
	FILES
		filestore/filestoretemplate.hpp
	DESTINATION
		include/ox/fs/filestore
)

install(
	FILES
		filestore/filestoretemplate.hpp
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
	#install(TARGETS oxfstool
	#		  RUNTIME DESTINATION bin
	#)
endif()
