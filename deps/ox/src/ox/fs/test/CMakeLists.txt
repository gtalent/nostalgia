cmake_minimum_required(VERSION 2.8)

add_executable(
	FSTests
		tests.cpp
)

target_link_libraries(
	FSTests
		OxFS
		OxStd
		OxTrace
		OxMetalClaw
)

add_test("Test\\ PathIterator::next1" FSTests PathIterator::next1)
add_test("Test\\ PathIterator::next2" FSTests PathIterator::next2)
add_test("Test\\ PathIterator::next3" FSTests PathIterator::next3)
add_test("Test\\ PathIterator::next4" FSTests PathIterator::next4)
add_test("Test\\ PathIterator::next5" FSTests PathIterator::next5)
add_test("Test\\ PathIterator::hasNext" FSTests PathIterator::hasNext)

add_test("Test\\ PathIterator::dirPath" FSTests PathIterator::dirPath)
add_test("Test\\ PathIterator::fileName" FSTests PathIterator::fileName)

add_test("Test\\ NodeBuffer::insert" FSTests "NodeBuffer::insert")
add_test("Test\\ FileStore::readWrite" FSTests "FileStore::readWrite")

add_test("Test\\ Directory" FSTests "Directory")
add_test("Test\\ FileSystem" FSTests "FileSystem")
