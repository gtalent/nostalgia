
add_library(
	NostalgiaCommon
		bounds.cpp
)

#install(TARGETS NostalgiaCommon DESTINATION lib)
install(
	FILES
		bounds.hpp
		common.hpp
		point.hpp
	DESTINATION
		include/nostalgia/common
)
