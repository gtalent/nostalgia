add_library(
	OxOrganicClaw
		read.cpp
		write.cpp
)

find_package(JsonCpp REQUIRED)

target_compile_options(OxOrganicClaw PRIVATE -Wsign-conversion)

target_link_libraries(
	OxOrganicClaw PUBLIC
		OxModel
		JsonCpp::JsonCpp
)

set_property(
	TARGET
		OxOrganicClaw
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		oc.hpp
		read.hpp
		write.hpp
	DESTINATION
		include/ox/oc
)

install(TARGETS OxOrganicClaw
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)

if(OX_RUN_TESTS STREQUAL "ON")
	add_subdirectory(test)
endif()
