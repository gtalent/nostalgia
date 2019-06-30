install(
	FILES
		nodebuffer.hpp
		ptr.hpp
	DESTINATION
		include/ox/ptrarith
)

if(OX_RUN_TESTS)
	add_subdirectory(test)
endif()
