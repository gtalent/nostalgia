add_library(
	OxMetalClaw
		defread.cpp
		deftypes.cpp
		defwriter.cpp
		presencemask.cpp
		read.cpp
		write.cpp
)

target_link_libraries(
	OxMetalClaw PUBLIC
		OxStd
)

set_property(
	TARGET
		OxMetalClaw
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		defread.hpp
		deftypes.hpp
		defwriter.hpp
		err.hpp
		optype.hpp
		presencemask.hpp
		read.hpp
		types.hpp
		walker.hpp
		write.hpp
	DESTINATION
		include/ox/mc
)

install(TARGETS OxMetalClaw
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)

if(OX_RUN_TESTS STREQUAL "ON")
	add_subdirectory(test)
endif()
