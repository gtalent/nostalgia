cmake_minimum_required(VERSION 2.8.8)

#setup libraries

if(WOMBAT_BUILD_TYPE STREQUAL "Native")
	find_package(SDL REQUIRED)
	include_directories(${SDL_INCLUDE_DIR})
	set(LIBS ${LIBS} ${SDL_LIBRARY})

	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIR})
	set(LIBS ${LIBS} ${OPENGL_LIBRARY})

	#find_package(SDL_IMAGE REQUIRED)
	#include_directories(${SDL_IMAGE_INCLUDE_DIR})
	#set(LIBS ${LIBS} ${SDL_IMAGE_LIBRARY})

	#find_package(SDL_ttf REQUIRED)
	#include_directories(${SDL_TTF_INCLUDE_DIR})
	#set(LIBS ${LIBS} ${SDL_TTF_LIBRARY})
elseif(WOMBAT_BUILD_TYPE STREQUAL "GBA")
endif()

find_package(Ox REQUIRED)
include_directories(${Ox_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OxFs_LIBRARY})

#project packages

include_directories(".")

add_subdirectory(core)
set(OBJS ${OBJS} $<TARGET_OBJECTS:NostalgiaCommon>)

add_subdirectory(common)
set(OBJS ${OBJS} $<TARGET_OBJECTS:NostalgiaCommon>)

add_subdirectory(player)
