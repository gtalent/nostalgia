/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// make sure asserts are enabled for the test file
#undef NDEBUG

#include <assert.h>
#include <map>
#include <vector>
#include <string>
#include <ox/std/std.hpp>
#include <ox/fs/filestore/filestoretemplate.hpp>
#include <ox/fs/filesystem/filesystem.hpp>


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
				auto buff = static_cast<char*>(ox_alloca(path.size() + 1));
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
				auto buff = static_cast<char*>(ox_alloca(path.size() + 1));
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
				auto buff = static_cast<char*>(ox_alloca(path.size() + 1));
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
				auto buff = static_cast<char*>(ox_alloca(path.size() + 1));
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
				auto buff = static_cast<char*>(ox_alloca(path.size() + 1));
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
				auto buff = static_cast<char*>(ox_alloca(path.size() + 1));
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
				auto buff = static_cast<char*>(ox_alloca(path.size() + 1));
				retval |= !(it.fileName(buff, path.size()) == 0 && ox_strcmp(buff, "charset.gbag") == 0);
				return retval;
			}
		},
		{
			"PathIterator::hasNext",
			[](string) {
				int retval = 0;
				const auto path = "/file1";
				PathIterator it(path, ox_strlen(path));
				oxAssert(it.hasNext(), "PathIterator shows incorrect hasNext");
				oxAssert(!it.next().hasNext(), "PathIterator shows incorrect hasNext");
				return retval;
			}
		},
		{
			"Ptr::subPtr",
			[](string) {
				constexpr auto buffLen = 5000;
				ox::ptrarith::Ptr<uint8_t, uint32_t> p(ox_alloca(buffLen), buffLen, 500, 500);
				oxAssert(p.valid(), "Ptr::subPtr: Ptr p is invalid.");

				auto subPtr = p.subPtr<uint64_t>(50);
				oxAssert(subPtr.valid(), "Ptr::subPtr: Ptr subPtr is invalid.");
				return 0;
			}
		},
		{
			"NodeBuffer::insert",
			[](string) {
				int err = 0;
				constexpr auto buffLen = 5000;
				auto list = new (ox_alloca(buffLen)) ox::ptrarith::NodeBuffer<uint32_t, ox::FileStoreItem<uint32_t>>(buffLen);
				oxAssert(list->malloc(50).valid(), "NodeBuffer::insert: malloc 1 failed");
				oxAssert(list->malloc(50).valid(), "NodeBuffer::insert: malloc 2 failed");
				auto first = list->firstItem();
				oxAssert(first.valid(), "NodeBuffer::insert: Could not access first item");
				oxAssert(first->size() == 50, "NodeBuffer::insert: First item size invalid");
				return err;
			}
		},
		{
			"FileStore::readWrite",
			[](string) {
				constexpr auto buffLen = 5000;
				constexpr auto str1 = "Hello, World!";
				constexpr auto str1Len = ox_strlen(str1) + 1;
				constexpr auto str2 = "Hello, Moon!";
				constexpr auto str2Len = ox_strlen(str2) + 1;
				auto list = new (ox_alloca(buffLen)) ox::ptrarith::NodeBuffer<uint32_t, ox::FileStoreItem<uint32_t>>(buffLen);
				oxAssert(ox::FileStore32::format(list, buffLen), "FileStore::format failed.");
				ox::FileStore32 fileStore(list, buffLen);
				oxAssert(fileStore.write(4, const_cast<char*>(str1), str1Len, 1), "FileStore::write 1 failed.");
				oxAssert(fileStore.write(5, const_cast<char*>(str2), str2Len, 1), "FileStore::write 2 failed.");

				char str1Read[str1Len];
				size_t str1ReadSize = 0;
				oxAssert(fileStore.read(4, reinterpret_cast<void*>(str1Read), str1Len, &str1ReadSize), "FileStore::read 1 failed.");

				return 0;
			}
		},
		{
			"Directory",
			[](string) {
				std::vector<uint8_t> fsBuff(5000);
				oxAssert(ox::FileStore32::format(fsBuff.data(), fsBuff.size()), "FS format failed");
				ox::FileStore32 fileStore(fsBuff.data(), fsBuff.size());
				ox::Directory32 dir(fileStore, 105);

				oxTrace("ox::fs::test::Directory") << "Init";
				oxAssert(dir.init(), "Init failed");

				oxTrace("ox::fs::test::Directory") << "write 1";
				oxAssert(dir.write("/file1", 1), "Directory write of file1 failed");

				oxTrace("ox::fs::test::Directory") << "find";
				oxAssert(dir.find("file1").error, "Could not find file1");
				oxAssert(dir.find("file1").value == 1, "Could not find file1");

				oxTrace("ox::fs::test::Directory") << "write 2";
				oxAssert(dir.write("/file3", 3), "Directory write of file3 failed");

				oxTrace("ox::fs::test::Directory") << "write 3";
				oxAssert(dir.write("/file2", 2), "Directory write of file2 failed");

				return 0;
			}
		},
		{
			"FileSystem",
			[](string) {
				std::vector<uint8_t> fsBuff(5000);
				oxTrace("ox::fs::test::FileSystem") << "format";
				oxAssert(ox::FileSystem32::format(fsBuff.data(), fsBuff.size()), "FileSystem format failed");
				ox::FileSystem32 fs(ox::FileStore32(fsBuff.data(), fsBuff.size()));

				oxTrace("ox::fs::test::FileSystem") << "mkdir";
				oxAssert(fs.mkdir("/dir", true), "mkdir failed");
				oxAssert(fs.stat("/dir").error, "mkdir failed");
				oxAssert(fs.mkdir("/l1d1/l2d1/l3d1", true), "mkdir failed");
				oxAssert(fs.stat("/l1d1/l2d1/l3d1").error, "mkdir failed");
				oxAssert(fs.mkdir("/l1d1/l2d2", true), "mkdir failed");
				oxAssert(fs.stat("/l1d1/l2d2").error, "mkdir failed");

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
