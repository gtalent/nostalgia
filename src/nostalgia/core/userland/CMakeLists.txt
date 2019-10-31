add_library(
	NostalgiaCore-Userspace
		media.cpp
		mem.cpp
)

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
