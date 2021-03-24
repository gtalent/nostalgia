add_executable(
	FSTests
		tests.cpp
)

target_link_libraries(
	FSTests
		OxFS
		OxStd
		OxMetalClaw
)

add_test("[ox/fs] PathIterator::next1" FSTests PathIterator::next1)
add_test("[ox/fs] PathIterator::next2" FSTests PathIterator::next2)
add_test("[ox/fs] PathIterator::next3" FSTests PathIterator::next3)
add_test("[ox/fs] PathIterator::next4" FSTests PathIterator::next4)
add_test("[ox/fs] PathIterator::next5" FSTests PathIterator::next5)
add_test("[ox/fs] PathIterator::hasNext" FSTests PathIterator::hasNext)

add_test("[ox/fs] PathIterator::dirPath" FSTests PathIterator::dirPath)
add_test("[ox/fs] PathIterator::fileName" FSTests PathIterator::fileName)

add_test("[ox/fs] NodeBuffer::insert" FSTests "NodeBuffer::insert")
add_test("[ox/fs] FileStore::readWrite" FSTests "FileStore::readWrite")

add_test("[ox/fs] Directory" FSTests "Directory")
add_test("[ox/fs] FileSystem" FSTests "FileSystem")
