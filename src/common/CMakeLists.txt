cmake_minimum_required(VERSION 2.8.8)

add_library(
	NostalgiaCommon
		bounds.cpp
		point.cpp
)

#install(TARGETS NostalgiaCommon DESTINATION lib)
install(
	FILES
		bounds.hpp
		common.hpp
		point.hpp
	DESTINATION
		include/wombat/common
)
