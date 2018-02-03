cmake_minimum_required(VERSION 2.8)

add_executable(
	FileStoreFormat
		filestore_format.cpp
)

add_executable(
	FileSystemFormat
		filesystem_format.cpp
)

add_executable(
	FileStoreIO
		filestoreio.cpp
)

add_executable(
	FSTests
		tests.cpp
)

target_link_libraries(
	FileStoreFormat
		OxFS
		OxStd
		OxTrace
)

target_link_libraries(
	FileSystemFormat
		OxFS
		OxStd
		OxTrace
)

target_link_libraries(
	FileStoreIO
		OxFS
		OxStd
		OxTrace
)

target_link_libraries(
	FSTests
		OxFS
		OxStd
		OxTrace
)

add_test("FileStoreFormat" FileStoreFormat)
add_test("FileSystemFormat" FileSystemFormat)
add_test("FileStoreIO" FileStoreIO)
add_test("Test\\ PathIterator::next1" FSTests PathIterator::next1)
add_test("Test\\ PathIterator::next2" FSTests PathIterator::next2)
add_test("Test\\ PathIterator::next3" FSTests PathIterator::next3)
add_test("Test\\ PathIterator::next4" FSTests PathIterator::next4)
add_test("Test\\ PathIterator::next5" FSTests PathIterator::next5)

add_test("Test\\ PathIterator::dirPath" FSTests PathIterator::dirPath)
add_test("Test\\ PathIterator::fileName" FSTests PathIterator::fileName)

add_test("Test\\ FileSystem32::findInodeOf\\ /" FSTests "FileSystem32::findInodeOf /")
add_test("Test\\ FileSystem32::write\\(string\\)" FSTests "FileSystem32::write(string)")
add_test("Test\\ FileSystem32::rmDirectoryEntry\\(string\\)" FSTests "FileSystem32::rmDirectoryEntry(string)")
add_test("Test\\ FileSystem32::remove\\(string,\\ true\\)" FSTests "FileSystem32::remove(string, true)")
add_test("Test\\ FileSystem32::move" FSTests "FileSystem32::move")
add_test("Test\\ FileSystem32::stripDirectories" FSTests "FileSystem32::stripDirectories")
add_test("Test\\ FileSystem32::ls" FSTests "FileSystem32::ls")
