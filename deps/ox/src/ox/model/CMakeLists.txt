add_library(
	OxModel
		desctypes.cpp
		descwrite.cpp
)

target_link_libraries(
	OxModel PUBLIC
		OxStd
)

set_property(
	TARGET
		OxModel
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		descread.hpp
		desctypes.hpp
		descwrite.hpp
		optype.hpp
		model.hpp
		types.hpp
		walk.hpp
	DESTINATION
		include/ox/model
)

install(TARGETS OxModel
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)
