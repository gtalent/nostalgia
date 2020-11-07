add_library(
	NostalgiaCore-Userspace
		media.cpp
)

if(NOT MSVC)
	target_compile_options(NostalgiaCore-Userspace PRIVATE -Wsign-conversion)
endif()

target_link_libraries(
	NostalgiaCore-Userspace PUBLIC
		OxFS
		OxStd
)

install(
	TARGETS
		NostalgiaCore-Userspace
	DESTINATION
		include/nostalgia/core
)
