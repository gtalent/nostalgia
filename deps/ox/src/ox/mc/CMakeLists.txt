add_library(
	OxMetalClaw
		presenceindicator.cpp
		read.cpp
		write.cpp
)

target_link_libraries(
	OxMetalClaw PUBLIC
		OxModel
		OxStd
)

if(NOT OX_BARE_METAL)
	set_property(
		TARGET
			OxMetalClaw
		PROPERTY
			POSITION_INDEPENDENT_CODE ON
	)
endif()

install(
	FILES
		intops.hpp
		err.hpp
		mc.hpp
		presenceindicator.hpp
		read.hpp
		types.hpp
		write.hpp
	DESTINATION
		include/ox/mc
)

install(TARGETS OxMetalClaw
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)

if(OX_RUN_TESTS)
	add_subdirectory(test)
endif()
