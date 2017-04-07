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
set(CMAKE_OBJCOPY ${DEVKITARM}/bin/arm-none-eabi-objcopy)
set(CMAKE_PADBIN ${DEVKITARM}/bin/padbin)
set(CMAKE_GBAFIX ${DEVKITARM}/bin/gbafix)
set(CMAKE_FIND_ROOT_PATH ${DEVKITARM})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

set(LINKER_FLAGS "-specs=gba.specs")
add_definitions (
	-DARM7
)

function(BuildStaticLib LIBNAME SRCFILES)
	add_library(${LIBNAME} OBJECT ${SRCFILES})
	set(OBJS ${OBJS} $<TARGET_OBJECTS:${LIBNAME}>)
endfunction()

include(FindPackageHandleStandardArgs)


macro(OBJCOPY_FILE EXE_NAME)
	set(FO ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.bin)
	set(FI ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME})
	message(STATUS ${FO})

	# run objcopy
	add_custom_command(
		OUTPUT "${FO}"
		COMMAND ${CMAKE_OBJCOPY}
		ARGS -O binary ${FI} ${FO}
		DEPENDS "${FI}"
	)

	get_filename_component(TGT "${EXE_NAME}" NAME)
	add_custom_target("TargetObjCopy_${TGT}" ALL DEPENDS ${FO} VERBATIM)
	get_directory_property(extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES)
	set_directory_properties(
		PROPERTIES
		ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files};${FO};"
	)
	set_source_files_properties("${FO}" PROPERTIES GENERATED TRUE)
endmacro(OBJCOPY_FILE)
