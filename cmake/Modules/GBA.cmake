set(CMAKE_SYSTEM_NAME "Generic")

set(DEVKITARM $ENV{DEVKITARM})
set(DEVKITPRO $ENV{DEVKITPRO})

if(NOT DEVKITPRO)
	message(FATAL_ERROR "DEVKITPRO environment variable not set")
endif()

if(NOT DEVKITARM)
	message(FATAL_ERROR "DEVKITARM environment variable not set")
endif()

set(CMAKE_C_COMPILER ${DEVKITARM}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${DEVKITARM}/bin/arm-none-eabi-g++)
set(CMAKE_FIND_ROOT_PATH ${DEVKITARM} ${DEVKITPRO}/libgba)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

set(LINKER_FLAGS "-specs=gba.specs")
add_definitions (
	-DARM7
)

find_library(GBA_LIBRARY
	NAMES
		gba
	PATHS
		/lib
)
find_path(GBA_INCLUDE_DIR gba.h
	PATHS
		/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GBA DEFAULT_MSG GBA_LIBRARY)
