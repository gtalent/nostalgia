/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/filestore/filestoretemplate.hpp>
#include <ox/fs/filesystem/filesystem.hpp>

#include "directory.hpp"

namespace ox::fs {

template<typename InodeId_t>
class FileSystemTemplate {
	private:
		static constexpr InodeId_t InodeFsData = 2;

		struct __attribute__((packed)) FileSystemData {
			LittleEndian<InodeId_t> rootDirInode;
		};

		FileStore *m_fs = nullptr;

	public:
		FileSystemTemplate(FileStore *fs);

		~FileSystemTemplate();

		Error format();

		Error mkdir(const char *path, bool recursive = false);

		/**
		 * Moves an entry from one directory to another.
		 * @param src the path to the file
		 * @param dest the path of the destination directory
		 */
		Error move(const char *src, const char *dest);

		Error read(const char *path, void *buffer, std::size_t buffSize);

		Error read(uint64_t inode, void *buffer, std::size_t size);

		Error read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size);

		Error remove(const char *path, bool recursive = false);

		void resize(uint64_t size = 0);

		Error write(const char *path, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile);

		Error write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile);

		FileStat stat(uint64_t inode);

		FileStat stat(const char *path);

		uint64_t spaceNeeded(uint64_t size);

		uint64_t available();

		uint64_t size();

		uint8_t *buff();

		void walk(int(*cb)(const char*, uint64_t, uint64_t));

		bool valid() const;

	private:
		ValErr<FileSystemData> fileSystemData();

};

template<typename InodeId_t>
FileSystemTemplate<InodeId_t>::FileSystemTemplate(FileStore *fs) {
	m_fs = fs;
}

template<typename InodeId_t>
FileSystemTemplate<InodeId_t>::~FileSystemTemplate() {
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::format() {
	oxReturnError(m_fs->format());

	constexpr auto rootDirInode = MaxValue<InodeId_t> / 2;
	auto rootDir = ox_malloca(sizeof(ox::fs::Directory<InodeId_t>), ox::fs::Directory<InodeId_t>, m_fs, rootDirInode);
	oxReturnError(rootDir->init());

	FileSystemData fd;
	fd.rootDirInode = rootDirInode;
	oxTrace("ox::fs::FileSystemTemplate::format") << "rootDirInode:" << fd.rootDirInode;
	oxReturnError(m_fs->write(InodeFsData, &fd, sizeof(fd)));

	if (m_fs->read(fd.rootDirInode).valid()) {
		oxTrace("ox::fs::FileSystemTemplate::format::error") << "FileSystemTemplate::format did not correctly create root directory";
	}

	return OxError(0);
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::mkdir(const char *path, bool recursive) {
	auto fd = fileSystemData();
	if (fd.ok()) {
		oxTrace("ox::fs::FileSystemTemplate::mkdir") << "rootDirInode:" << fd.value.rootDirInode;
		auto rootDir = ox_malloca(sizeof(ox::fs::Directory<InodeId_t>), ox::fs::Directory<InodeId_t>, m_fs, fd.value.rootDirInode);
		return rootDir->mkdir(path, recursive);
	} else {
		oxLogError(fd.error);
		return fd.error;
	}
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::move(const char *src, const char *dest) {
	auto fd = fileSystemData();
	if (fd.ok()) {
		auto rootDir = ox_malloca(sizeof(ox::fs::Directory<InodeId_t>), ox::fs::Directory<InodeId_t>, m_fs, fd.value.rootDirInode);
		auto inode = rootDir->find(src);
		oxReturnError(inode.error);
		oxReturnError(rootDir->write(dest, inode));
		oxReturnError(rootDir->remove(src));
		return OxError(0);
	} else {
		return fd.error;
	}
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::read(const char *path, void *buffer, std::size_t buffSize) {
	auto fd = fileSystemData();
	if (fd.ok()) {
		auto rootDir = ox_malloca(sizeof(ox::fs::Directory<InodeId_t>), ox::fs::Directory<InodeId_t>, m_fs, fd.value.rootDirInode);
		auto inode = rootDir->find(path);
		oxReturnError(inode.error);
		return read(inode, buffer, buffSize);
	} else {
		return fd.error;
	}
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::read(uint64_t inode, void *buffer, std::size_t buffSize) {
	return m_fs->read(inode, buffer, buffSize);
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) {
	return m_fs->read(inode, readStart, readSize, buffer, size);
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::remove(const char *path, bool recursive) {
	auto fd = fileSystemData();
	if (fd.ok()) {
		auto rootDir = ox_malloca(sizeof(ox::fs::Directory<InodeId_t>), ox::fs::Directory<InodeId_t>, m_fs, fd.value.rootDirInode);
		auto inode = rootDir->find(path);
		oxReturnError(inode.error);
		if (auto err = rootDir->remove(path)) {
			// removal failed, try putting the index back
			oxLogError(rootDir->write(path, inode));
			return err;
		}
		return OxError(0);
	} else {
		return fd.error;
	}
}

template<typename InodeId_t>
void FileSystemTemplate<InodeId_t>::resize(uint64_t size) {
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::write(const char *path, void *buffer, uint64_t size, uint8_t fileType) {
	return OxError(0);
}

template<typename InodeId_t>
Error FileSystemTemplate<InodeId_t>::write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) {
	return OxError(0);
}

template<typename InodeId_t>
FileStat FileSystemTemplate<InodeId_t>::stat(uint64_t inode) {
	return {};
}

template<typename InodeId_t>
FileStat FileSystemTemplate<InodeId_t>::stat(const char *path) {
	return {};
}

template<typename InodeId_t>
uint64_t FileSystemTemplate<InodeId_t>::spaceNeeded(uint64_t size) {
	return 0;
}

template<typename InodeId_t>
uint64_t FileSystemTemplate<InodeId_t>::available() {
	return 0;
}

template<typename InodeId_t>
uint64_t FileSystemTemplate<InodeId_t>::size() {
	return 0;
}

template<typename InodeId_t>
uint8_t *FileSystemTemplate<InodeId_t>::buff() {
	return nullptr;
}

template<typename InodeId_t>
void FileSystemTemplate<InodeId_t>::walk(int(*cb)(const char*, uint64_t, uint64_t)) {
}

template<typename InodeId_t>
ValErr<typename FileSystemTemplate<InodeId_t>::FileSystemData> FileSystemTemplate<InodeId_t>::fileSystemData() {
	FileSystemData fd;
	auto err = m_fs->read(InodeFsData, &fd, sizeof(fd));
	if (err != 0) {
		return {fd, err};
	}
	return fd;
}

extern template class Directory<uint16_t>;
extern template class Directory<uint32_t>;

extern template class FileSystemTemplate<uint16_t>;
extern template class FileSystemTemplate<uint32_t>;

using FileSystem16 = FileSystemTemplate<uint16_t>;
using FileSystem32 = FileSystemTemplate<uint32_t>;

}
