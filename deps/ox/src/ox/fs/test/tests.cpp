/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// make sure asserts are enabled for the test file
#undef NDEBUG

#include <functional>
#include <map>
#include <string_view>
#include <ox/std/std.hpp>
#include <ox/fs/ptrarith/nodebuffer.hpp>
#include <ox/fs/filestore/filestoretemplate.hpp>
#include <ox/fs/filesystem/filesystem.hpp>

template<typename T>
struct OX_PACKED NodeType: public ox::ptrarith::Item<T> {
	public:
		size_t fullSize() const {
			return this->size() + sizeof(*this);
		}
};

const std::map<std::string_view, std::function<ox::Error(std::string_view)>> tests = {
	{
		{
			"PtrArith::setSize",
			[](std::string_view) {
				using BuffPtr_t = uint32_t;
				ox::Vector<char> buff(5 * ox::units::MB);
				auto buffer = new (buff.data()) ox::ptrarith::NodeBuffer<BuffPtr_t, NodeType<BuffPtr_t>>(buff.size());
				using String = ox::BString<6>;
				auto a1 = buffer->malloc(sizeof(String));
				auto a2 = buffer->malloc(sizeof(String));
				oxAssert(a1.valid(), "Allocation 1 failed.");
				oxAssert(a2.valid(), "Allocation 2 failed.");
				auto s1Buff = buffer->dataOf<String>(a1);
				auto s2Buff = buffer->dataOf<String>(a2);
				oxAssert(s1Buff.valid(), "s1 allocation 1 failed.");
				oxAssert(s2Buff.valid(), "s2 allocation 2 failed.");
				auto &s1 = *new (s1Buff) String("asdf");
				auto &s2 = *new (s2Buff) String("aoeu");
				oxTrace("test") << "s1: " << s1.c_str();
				oxTrace("test") << "s2: " << s2.c_str();
				oxAssert(s1 == "asdf", "Allocation 1 not as expected.");
				oxAssert(s2 == "aoeu", "Allocation 2 not as expected.");
				oxAssert(buffer->free(a1), "Free failed.");
				oxAssert(buffer->free(a2), "Free failed.");
				oxAssert(buffer->setSize(buffer->size() - buffer->available()), "Resize failed.");
				return OxError(0);
			}
		},
		{
			"PathIterator::next1",
			[](std::string_view) {
				ox::String path = "/usr/share/charset.gbag";
				ox::PathIterator it(path.c_str(), path.len());
				auto buff = static_cast<char*>(ox_alloca(path.len() + 1));
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "usr") == 0, "PathIterator shows wrong next");
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "share") == 0, "PathIterator shows wrong next");
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "charset.gbag") == 0, "PathIterator shows wrong next");
				return OxError(0);
			}
		},
		{
			"PathIterator::next2",
			[](std::string_view) {
				ox::String path = "/usr/share/";
				ox::PathIterator it(path.c_str(), path.len());
				auto buff = static_cast<char*>(ox_alloca(path.len() + 1));
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "usr") == 0, "PathIterator shows wrong next");
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "share") == 0, "PathIterator shows wrong next");
				return OxError(0);
			}
		},
		{
			"PathIterator::next3",
			[](std::string_view) {
				ox::String path = "/";
				ox::PathIterator it(path.c_str(), path.len());
				auto buff = static_cast<char*>(ox_alloca(path.len() + 1));
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "\0") == 0, "PathIterator shows wrong next");
				return OxError(0);
			}
		},
		{
			"PathIterator::next4",
			[](std::string_view) {
				ox::String path = "usr/share/charset.gbag";
				ox::PathIterator it(path.c_str(), path.len());
				auto buff = static_cast<char*>(ox_alloca(path.len() + 1));
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "usr") == 0, "PathIterator shows wrong next");
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "share") == 0, "PathIterator shows wrong next");
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "charset.gbag") == 0, "PathIterator shows wrong next");
				return OxError(0);
			}
		},
		{
			"PathIterator::next5",
			[](std::string_view) {
				ox::String path = "usr/share/";
				ox::PathIterator it(path.c_str(), path.len());
				auto buff = static_cast<char*>(ox_alloca(path.len() + 1));
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "usr") == 0, "PathIterator shows wrong next");
				oxAssert(it.next(buff, path.len()) == 0 && ox_strcmp(buff, "share") == 0, "PathIterator shows wrong next");
				return OxError(0);
			}
		},
		{
			"PathIterator::dirPath",
			[] (std::string_view) {
				ox::String path = "/usr/share/charset.gbag";
				ox::PathIterator it(path.c_str(), path.len());
				auto buff = static_cast<char*>(ox_alloca(path.len() + 1));
				oxAssert(it.dirPath(buff, path.len()) == 0 && ox_strcmp(buff, "/usr/share/") == 0, "PathIterator shows incorrect dir path");
				return OxError(0);
			}
		},
		{
			"PathIterator::fileName",
			[](std::string_view) {
				ox::String path = "/usr/share/charset.gbag";
				ox::PathIterator it(path.c_str(), path.len());
				auto buff = static_cast<char*>(ox_alloca(path.len() + 1));
				oxAssert(it.fileName(buff, path.len()) == 0 && ox_strcmp(buff, "charset.gbag") == 0, "PathIterator shows incorrect file name");
				return OxError(0);
			}
		},
		{
			"PathIterator::hasNext",
			[](std::string_view) {
				const auto path = "/file1";
				ox::PathIterator it(path, ox_strlen(path));
				oxAssert(it.hasNext(), "PathIterator shows incorrect hasNext");
				oxAssert(!it.next().hasNext(), "PathIterator shows incorrect hasNext");
				return OxError(0);
			}
		},
		{
			"Ptr::subPtr",
			[](std::string_view) {
				constexpr auto buffLen = 5000;
				ox::ptrarith::Ptr<uint8_t, uint32_t> p(ox_alloca(buffLen), buffLen, 500, 500);
				oxAssert(p.valid(), "Ptr::subPtr: Ptr p is invalid.");

				auto subPtr = p.subPtr<uint64_t>(50);
				oxAssert(subPtr.valid(), "Ptr::subPtr: Ptr subPtr is invalid.");
				return OxError(0);
			}
		},
		{
			"NodeBuffer::insert",
			[](std::string_view) {
				constexpr auto buffLen = 5000;
				auto list = new (ox_alloca(buffLen)) ox::ptrarith::NodeBuffer<uint32_t, ox::FileStoreItem<uint32_t>>(buffLen);
				oxAssert(list->malloc(50).valid(), "NodeBuffer::insert: malloc 1 failed");
				oxAssert(list->malloc(50).valid(), "NodeBuffer::insert: malloc 2 failed");
				auto first = list->firstItem();
				oxAssert(first.valid(), "NodeBuffer::insert: Could not access first item");
				oxAssert(first->size() == 50, "NodeBuffer::insert: First item size invalid");
				return OxError(0);
			}
		},
		{
			"FileStore::readWrite",
			[](std::string_view) {
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

				return OxError(0);
			}
		},
		{
			"Directory",
			[](std::string_view) {
				ox::Vector<uint8_t> fsBuff(5000);
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

				return OxError(0);
			}
		},
		{
			"FileSystem",
			[](std::string_view) {
				ox::Vector<uint8_t> fsBuff(5000);
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

				return OxError(0);
			}
		},
	},
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		std::string_view testName = args[1];
		std::string_view testArg = "";
		if (args[2]) {
			testArg = args[2];
		}
		if (tests.find(testName) != tests.end()) {
			retval = tests.at(testName)(testArg);
		}
	}
	return retval;
}
