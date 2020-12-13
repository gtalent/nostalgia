
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(
	NostalgiaCore-Qt SHARED
		gfx.cpp
)

target_link_libraries(
	NostalgiaCore-Qt PUBLIC
		NostalgiaStudio
		OxFS
		OxStd
)

install(
	TARGETS
		NostalgiaCore-Qt
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_LIB}/nostalgia
)
