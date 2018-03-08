/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// make sure asserts are enabled for the test file
#undef NDEBUG

#include <iostream>
#include <assert.h>
#include <map>
#include <vector>
#include <string>
#include <ox/fs/fs.hpp>
#include <ox/std/std.hpp>
#include <ox/fs/filestore/filestore.hpp>
#include <ox/fs/filestore/filestoretemplate.hpp>


using namespace std;
using namespace ox;

map<string, int(*)(string)> tests = {
	{
		{
			"PathIterator::next1",
			[](string) {
				int retval = 0;
				string path = "/usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "charset.gbag") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next2",
			[](string) {
				int retval = 0;
				string path = "/usr/share/";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next3",
			[](string) {
				int retval = 0;
				string path = "/";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "\0") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next4",
			[](string) {
				int retval = 0;
				string path = "usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "charset.gbag") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next5",
			[](string) {
				int retval = 0;
				string path = "usr/share/";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				return retval;
			}
		},
		{
			"PathIterator::dirPath",
			[] (string) {
				int retval = 0;
				string path = "/usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.dirPath(buff, path.size()) == 0 && ox_strcmp(buff, "/usr/share/") == 0);
				return retval;
			}
		},
		{
			"PathIterator::fileName",
			[](string) {
				int retval = 0;
				string path = "/usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.fileName(buff, path.size()) == 0 && ox_strcmp(buff, "charset.gbag") == 0);
				return retval;
			}
		},
		{
			"FileSystem32::findInodeOf /",
			[](string) {
				int retval = 0;
				const auto size = 1024;
				uint8_t buff[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);
				retval |= !(fs->findInodeOf("/") == FileSystem32::INODE_ROOT_DIR);
				delete fs;
				return retval;
			}
		},
		{
			"FileSystem32::write(string)",
			[](string) {
				int retval = 0;
				auto path = "/usr/share/test.txt";
				auto dataIn = "test string";
				auto dataOutLen = ox_strlen(dataIn) + 1;
				auto dataOut = new char[dataOutLen];

				const auto size = 1024 * 1024;
				auto buff = new uint8_t[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);

				retval |= fs->mkdir("/usr");
				retval |= fs->mkdir("/usr/share");
				retval |= fs->mkdir("/usr/lib");

				retval |= fs->write(path, (void*) dataIn, ox_strlen(dataIn) + 1);
				retval |= fs->read(path, dataOut, dataOutLen);
				retval |= ox_strcmp(dataIn, dataOut) != 0;

				delete fs;
				delete []buff;
				delete []dataOut;

				return retval;
			}
		},
		{
			"FileSystem32::rmDirectoryEntry(string)",
			[](string) {
				int retval = 0;
				auto path = "/usr/share/test.txt";
				auto dataIn = "test string";
				auto dataOutLen = ox_strlen(dataIn) + 1;
				auto dataOut = new char[dataOutLen];

				const auto size = 1024 * 1024 * 10;
				auto buff = new uint8_t[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);

				retval |= fs->mkdir("/usr");
				retval |= fs->mkdir("/usr/share");

				retval |= fs->write(path, (void*) dataIn, ox_strlen(dataIn) + 1);
				retval |= fs->read(path, dataOut, dataOutLen);
				retval |= ox_strcmp(dataIn, dataOut) != 0;

				retval |= fs->rmDirectoryEntry(path);
				// the lookup should fail
				retval |= fs->read(path, dataOut, dataOutLen) == 0;

				delete fs;
				delete []buff;
				delete []dataOut;

				return retval;
			}
		},
		{
			"FileSystem32::remove(string, true)",
			[](string) {
				int retval = 0;
				auto dataIn = "test string";
				auto dataOutLen = 1024 * 64;
				auto dataOut = new char[dataOutLen];
				std::vector<uint64_t> inodes;

				const auto size = 1024 * 1024;
				auto buff = new uint8_t[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);

				retval |= fs->mkdir("/usr");
				retval |= fs->mkdir("/usr/share");
				retval |= fs->write("/usr/share/test.txt", (void*) dataIn, ox_strlen(dataIn) + 1);

				inodes.push_back(fs->stat("/usr").inode);
				inodes.push_back(fs->stat("/usr/share").inode);
				inodes.push_back(fs->stat("/usr/share/test.txt").inode);

				retval |= fs->remove("/usr", true);

				// the lookup should fail
				for (auto inode : inodes) {
					retval |= fs->read(inode, dataOut, dataOutLen) == 0;
				}

				delete fs;
				delete []buff;
				delete []dataOut;

				return retval;
			}
		},
		{
			"FileSystem32::move",
			[](string) {
				int retval = 0;
				auto dataIn = "test string";
				auto dataOutLen = ox_strlen(dataIn) + 1;
				auto dataOut = new char[dataOutLen];
				std::vector<uint64_t> inodes;

				const auto size = 1024 * 1024;
				auto buff = new uint8_t[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);

				retval |= fs->mkdir("/usr");
				retval |= fs->mkdir("/usr/share");
				retval |= fs->write("/usr/share/test.txt", (void*) dataIn, ox_strlen(dataIn) + 1);

				retval |= fs->move("/usr/share", "/share");
				retval |= fs->read("/share/test.txt", dataOut, dataOutLen);
				retval |= !(ox_strcmp(dataIn, dataOut) == 0);

				delete fs;
				delete []buff;
				delete []dataOut;

				return retval;
			}
		},
		{
			"FileSystem32::stripDirectories",
			[](string) {
				int retval = 0;
				auto dataIn = "test string";
				auto dataOutLen = ox_strlen(dataIn) + 1;
				auto dataOut = new char[dataOutLen];
				std::vector<uint64_t> inodes;

				const auto size = 1024 * 1024;
				auto buff = new uint8_t[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);

				retval |= fs->mkdir("/usr");
				retval |= fs->mkdir("/usr/share");
				retval |= fs->write("/usr/share/test.txt", (void*) dataIn, ox_strlen(dataIn) + 1);

				auto inode = fs->stat("/usr/share/test.txt").inode;

				retval |= fs->stripDirectories();

				// make sure normal file is still there and the directories are gone
				retval |= fs->read(inode, dataOut, dataOutLen);
				retval |= !(ox_strcmp(dataIn, dataOut) == 0);
				retval |= !(fs->stat("/usr").inode == 0);
				retval |= !(fs->stat("/usr/share").inode == 0);

				delete fs;
				delete []buff;
				delete []dataOut;

				return retval;
			}
		},
		{
			"FileSystem32::ls",
			[](string) {
				int retval = 0;
				auto dataIn = "test string";
				auto dataOutLen = ox_strlen(dataIn) + 1;
				auto dataOut = new char[dataOutLen];
				std::vector<uint64_t> inodes;
				std::vector<DirectoryListing<string>> files;

				const auto size = 1024 * 1024;
				auto buff = new uint8_t[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);

				retval |= fs->mkdir("/usr");
				retval |= fs->mkdir("/usr/share");
				retval |= fs->write("/usr/share/a.txt", (void*) dataIn, ox_strlen(dataIn) + 1);
				retval |= fs->write("/usr/share/b.txt", (void*) dataIn, ox_strlen(dataIn) + 1);
				retval |= fs->write("/usr/share/c.txt", (void*) dataIn, ox_strlen(dataIn) + 1);

				fs->ls("/usr/share/", &files);

				retval |= !(files[0].name == ".");
				retval |= !(files[1].name == "..");
				retval |= !(files[2].name == "a.txt");
				retval |= !(files[3].name == "b.txt");
				retval |= !(files[4].name == "c.txt");

				delete fs;
				delete []buff;
				delete []dataOut;

				return retval;
			}
		},
		{
			"LinkedList::insert",
			[](string) {
				int err = 0;
				constexpr auto buffLen = 5000;
				uint8_t buff[buffLen];
				auto list = new (buff) ox::fs::LinkedList<uint32_t, ox::fs::Item>(buffLen);
				err |= !(list->malloc(50).valid());
				err |= !(list->firstItem().valid());
				err |= !(list->firstItem()->size() == 50);
				return err;
			}
		},
		{
			"FileStore::readWrite",
			[](string) {
				constexpr auto buffLen = 5000;
				uint8_t buff[buffLen];
				auto list = new (buff) ox::fs::LinkedList<uint32_t, ox::fs::Item>(buffLen);
				ox::fs::FileStore32 fileStore(list, buffLen);
				ox_assert(fileStore.format() == 0, "Filestore::format failed.");
				return 0;
			}
		},
	},
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		auto testName = args[1];
		string testArg = "";
		if (args[2]) {
			testArg = args[2];
		}
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName](testArg);
		}
	}
	return retval;
}
