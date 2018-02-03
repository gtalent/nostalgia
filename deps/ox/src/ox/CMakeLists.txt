cmake_minimum_required(VERSION 2.8)

if(OX_USE_STDLIB STREQUAL "ON")
	add_subdirectory(clargs)
endif(OX_USE_STDLIB STREQUAL "ON")
add_subdirectory(fs)
add_subdirectory(trace)
add_subdirectory(mc)
add_subdirectory(std)
