
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
	if(NOT APPLE AND NOT MSVC)
		target_link_libraries(
			OxFS PUBLIC
				stdc++fs
		)
	endif()
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

install(
	FILES
		filestore/filestoretemplate.hpp
	DESTINATION
		include/ox/fs/filestore
)

install(
	FILES
		filesystem/filesystem.hpp
		filesystem/pathiterator.hpp
	DESTINATION
		include/ox/fs/filesystem
)

install(
	FILES
		ptrarith/nodebuffer.hpp
		ptrarith/ptr.hpp
	DESTINATION
		include/ox/fs/ptrarith
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
