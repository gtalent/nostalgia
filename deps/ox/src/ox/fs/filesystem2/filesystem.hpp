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

/**
 * FileSystemTemplate used to create file system that wraps around a FileStore,
 * taking an inode size and a directory type as parameters.
 *
 * Note: Directory parameter must have a default constructor.
 */
template<typename InodeId_t, typename Directory>
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

		void resize(uint64_t size, void *buffer = nullptr);

		Error write(const char *path, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile);

		Error write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile);

		ValErr<FileStat> stat(uint64_t inode);

		ValErr<FileStat> stat(const char *path);

		uint64_t spaceNeeded(uint64_t size);

		uint64_t available();

		uint64_t size();

		uint8_t *buff();

		Error walk(Error(*cb)(uint8_t, uint64_t, uint64_t));

		bool valid() const;

	private:
		ValErr<FileSystemData> fileSystemData() const noexcept;

		/**
		 * Finds the inode ID at the given path.
		 */
		ValErr<uint64_t> find(const char *path) const noexcept;

		ValErr<Directory> rootDir() const noexcept;

};

template<typename InodeId_t, typename Directory>
FileSystemTemplate<InodeId_t, Directory>::FileSystemTemplate(FileStore *fs) {
	m_fs = fs;
}

template<typename InodeId_t, typename Directory>
FileSystemTemplate<InodeId_t, Directory>::~FileSystemTemplate() {
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::format() {
	oxReturnError(m_fs->format());

	constexpr auto rootDirInode = MaxValue<InodeId_t> / 2;
	Directory rootDir(m_fs, rootDirInode);
	oxReturnError(rootDir.init());

	FileSystemData fd;
	fd.rootDirInode = rootDirInode;
	oxTrace("ox::fs::FileSystemTemplate::format") << "rootDirInode:" << fd.rootDirInode;
	oxReturnError(m_fs->write(InodeFsData, &fd, sizeof(fd)));

	if (m_fs->read(fd.rootDirInode).valid()) {
		oxTrace("ox::fs::FileSystemTemplate::format::error") << "FileSystemTemplate::format did not correctly create root directory";
	}

	return OxError(0);
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::mkdir(const char *path, bool recursive) {
	auto fd = fileSystemData();
	if (fd.ok()) {
		oxTrace("ox::fs::FileSystemTemplate::mkdir") << "rootDirInode:" << fd.value.rootDirInode;
		Directory rootDir(m_fs, fd.value.rootDirInode);
		return rootDir.mkdir(path, recursive);
	} else {
		oxLogError(fd.error);
		return fd.error;
	}
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::move(const char *src, const char *dest) {
	auto fd = fileSystemData();
	oxReturnError(fd.error);
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto inode = rootDir.find(src);
	oxReturnError(inode.error);
	oxReturnError(rootDir.write(dest, inode));
	oxReturnError(rootDir.remove(src));
	return OxError(0);
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::read(const char *path, void *buffer, std::size_t buffSize) {
	auto fd = fileSystemData();
	oxReturnError(fd.error);
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto inode = rootDir.find(path);
	oxReturnError(inode.error);
	return read(inode, buffer, buffSize);
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::read(uint64_t inode, void *buffer, std::size_t buffSize) {
	return m_fs->read(inode, buffer, buffSize);
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) {
	return m_fs->read(inode, readStart, readSize, buffer, size);
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::remove(const char *path, bool recursive) {
	auto fd = fileSystemData();
	oxReturnError(fd.error);
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto inode = rootDir.find(path);
	oxReturnError(inode.error);
	auto st = stat(inode);
	oxReturnError(st.error);
	if (st.value.fileType == FileType_NormalFile || recursive) {
		if (auto err = rootDir.remove(path)) {
			// removal failed, try putting the index back
			oxLogError(rootDir.write(path, inode));
			return err;
		}
	} else {
		oxTrace("FileSystemTemplate::remove::fail") << "Tried to remove directory without recursive setting.";
		return OxError(1);
	}
	return OxError(0);
}

template<typename InodeId_t, typename Directory>
void FileSystemTemplate<InodeId_t, Directory>::resize(uint64_t size, void *buffer) {
	m_fs->resize(size, buffer);
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::write(const char *path, void *buffer, uint64_t size, uint8_t fileType) {
	auto inode = find(path);
	if (inode.error) {
		inode = m_fs->generateInodeId();
	}
	auto rootDir = this->rootDir();
	oxReturnError(rootDir.error);
	oxReturnError(rootDir.value.write(path, inode));
	oxReturnError(write(inode, buffer, size, fileType));
	return 0;
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) {
	return m_fs->write(inode, buffer, size, fileType);
}

template<typename InodeId_t, typename Directory>
ValErr<FileStat> FileSystemTemplate<InodeId_t, Directory>::stat(uint64_t inode) {
	auto s = m_fs->stat(inode);
	FileStat out;
	out.inode = s.value.inode;
	out.links = s.value.links;
	out.size = s.value.size;
	out.fileType = s.value.fileType;
	return {out, s.error};
}

template<typename InodeId_t, typename Directory>
ValErr<FileStat> FileSystemTemplate<InodeId_t, Directory>::stat(const char *path) {
	auto inode = find(path);
	if (inode.error) {
		return {{}, inode.error};
	}
	return stat(inode.value);
}

template<typename InodeId_t, typename Directory>
uint64_t FileSystemTemplate<InodeId_t, Directory>::spaceNeeded(uint64_t size) {
	return m_fs->spaceNeeded(size);
}

template<typename InodeId_t, typename Directory>
uint64_t FileSystemTemplate<InodeId_t, Directory>::available() {
	return m_fs->available();
}

template<typename InodeId_t, typename Directory>
uint64_t FileSystemTemplate<InodeId_t, Directory>::size() {
	return m_fs->size();
}

template<typename InodeId_t, typename Directory>
uint8_t *FileSystemTemplate<InodeId_t, Directory>::buff() {
	return m_fs->buff();
}

template<typename InodeId_t, typename Directory>
Error FileSystemTemplate<InodeId_t, Directory>::walk(Error(*cb)(uint8_t, uint64_t, uint64_t)) {
	return m_fs->walk(cb);
}

template<typename InodeId_t, typename Directory>
ValErr<typename FileSystemTemplate<InodeId_t, Directory>::FileSystemData> FileSystemTemplate<InodeId_t, Directory>::fileSystemData() const noexcept {
	FileSystemData fd;
	auto err = m_fs->read(InodeFsData, &fd, sizeof(fd));
	if (err != 0) {
		return {fd, err};
	}
	return fd;
}

template<typename InodeId_t, typename Directory>
ValErr<uint64_t> FileSystemTemplate<InodeId_t, Directory>::find(const char *path) const noexcept {
	auto fd = fileSystemData();
	if (fd.error) {
		return {0, fd.error};
	}
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto inode = rootDir.find(path);
	if (inode.error) {
		return {0, inode.error};
	}
	return inode.value;
}

template<typename InodeId_t, typename Directory>
ValErr<Directory> FileSystemTemplate<InodeId_t, Directory>::rootDir() const noexcept {
	auto fd = fileSystemData();
	if (fd.error) {
		return {{}, fd.error};
	}
	return Directory(m_fs, fd.value.rootDirInode);
}

extern template class Directory<uint16_t>;
extern template class Directory<uint32_t>;

extern template class FileSystemTemplate<uint16_t, Directory<uint16_t>>;
extern template class FileSystemTemplate<uint32_t, Directory<uint16_t>>;

using FileSystem16 = FileSystemTemplate<uint16_t, Directory<uint16_t>>;
using FileSystem32 = FileSystemTemplate<uint32_t, Directory<uint32_t>>;

}
