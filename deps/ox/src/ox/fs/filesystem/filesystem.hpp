/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/filestore.hpp>

#include "directory.hpp"
#include "pathiterator.hpp"

namespace ox {

class FileSystem {
	public:
		virtual ~FileSystem() {};

		virtual int stripDirectories() = 0;

		virtual int mkdir(const char *path, bool recursive = false) = 0;

		/**
		 * Moves an entry from one directory to another.
		 * @param src the path to the file
		 * @param dest the path of the destination directory
		 */
		virtual int move(const char *src, const char *dest) = 0;

		template<typename List>
		int ls(const char *path, List *list);

		virtual int read(const char *path, void *buffer, size_t buffSize) = 0;

		virtual int read(uint64_t inode, void *buffer, size_t size) = 0;

		virtual int read(uint64_t inode, size_t readStart, size_t readSize, void *buffer, size_t *size) = 0;

		virtual uint8_t *read(uint64_t inode, size_t *size) = 0;

		virtual int remove(uint64_t inode, bool recursive = false) = 0;

		virtual int remove(const char *path, bool recursive = false) = 0;

		virtual void resize(uint64_t size = 0) = 0;

		virtual int write(const char *path, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) = 0;

		virtual int write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) = 0;

		virtual FileStat stat(uint64_t inode) = 0;

		virtual FileStat stat(const char *path) = 0;

		virtual uint64_t spaceNeeded(uint64_t size) = 0;

		virtual uint64_t available() = 0;

		virtual uint64_t size() = 0;

		virtual uint8_t *buff() = 0;

		virtual void walk(int(*cb)(const char*, uint64_t, uint64_t)) = 0;

	protected:
		virtual int readDirectory(const char *path, Directory<uint64_t, uint64_t> *dirOut) = 0;
};

template<typename List>
int FileSystem::ls(const char *path, List *list) {
	typedef Directory<uint64_t, uint64_t> Dir;
	int err = 0;
	auto s = stat(path);
	if (s.fileType == FileType_Directory) {
		uint8_t dirBuff[max(static_cast<size_t>(s.size), sizeof(Dir)) * 4];
		auto dir = (Directory<uint64_t, uint64_t>*) dirBuff;
		err |= readDirectory(path, dir);
		if (!err) {
			err |= dir->ls(list);
		}
	}
	return err;
}

FileSystem *createFileSystem(uint8_t *buff, size_t buffSize, bool ownsBuff = false);

/**
 * Creates a larger version of the given FileSystem.
 */
FileSystem *expandCopy(FileSystem *src);

/**
 * Calls expandCopy and deletes the original FileSystem and buff a resize was
 * performed.
 */
FileSystem *expandCopyCleanup(FileSystem *fs, size_t size);

}
