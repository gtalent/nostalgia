add_library(
	OxSerialization
		desctypes.cpp
		descwrite.cpp
)

target_link_libraries(
	OxSerialization PUBLIC
		OxStd
)

set_property(
	TARGET
		OxSerialization
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		descread.hpp
		desctypes.hpp
		descwrite.hpp
		optype.hpp
		ser.hpp
		types.hpp
		walk.hpp
	DESTINATION
		include/ox/ser
)

install(TARGETS OxSerialization
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)
