set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
	NostalgiaTool SHARED
		pack.cpp
		tilesetimport.cpp
)

target_link_libraries(
	NostalgiaTool PUBLIC
		Qt5::Widgets
		OxClArgs
		OxFS
		OxStd
		NostalgiaCore
)

install(
	FILES
		pack.hpp
		tilesetimport.hpp
	DESTINATION
		include/nostalgia/tools/lib
)

install(
	TARGETS
		NostalgiaTool
	LIBRARY DESTINATION ${NOSTALGIA_DIST_LIB}/nostalgia
	ARCHIVE DESTINATION ${NOSTALGIA_DIST_LIB}/nostalgia
)
