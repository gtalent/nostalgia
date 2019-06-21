
add_library(
	OxFS
		filestore/filestoretemplate.cpp
		filesystem/filelocation.cpp
		filesystem/pathiterator.cpp
		filesystem/directory.cpp
		filesystem/filesystem.cpp
		filesystem/passthroughfs.cpp
)

if(NOT OX_BARE_METAL)
	target_link_libraries(
		OxFS PUBLIC
			$<$<CXX_COMPILER_ID:Clang>:c++fs>
			$<$<CXX_COMPILER_ID:GNU>:stdc++fs>
	)
	set_property(
		TARGET
			OxFS
		PROPERTY
			POSITION_INDEPENDENT_CODE ON
	)
endif()

target_link_libraries(
	OxFS PUBLIC
		OxMetalClaw
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

if(OX_RUN_TESTS)
	add_subdirectory(test)
endif()
