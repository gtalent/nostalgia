/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/filestore/filestoretemplate.hpp>
#include <ox/fs/filesystem/filelocation.hpp>
#include <ox/fs/filesystem/types.hpp>

#include "directory.hpp"

namespace ox {

class FileSystem {

	public:
		virtual ~FileSystem() = default;

		[[nodiscard]] virtual ox::Error mkdir(const char *path, bool recursive = false) = 0;

		/**
		 * Moves an entry from one directory to another.
		 * @param src the path to the file
		 * @param dest the path of the destination directory
		 */
		[[nodiscard]] virtual ox::Error move(const char *src, const char *dest) = 0;

		[[nodiscard]] virtual ox::Error read(const char *path, void *buffer, std::size_t buffSize) = 0;

		[[nodiscard]] virtual ox::ValErr<uint8_t*> read(const char *path) = 0;

		[[nodiscard]] virtual ox::Error read(uint64_t inode, void *buffer, std::size_t size) = 0;

		[[nodiscard]] virtual ox::Error read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) = 0;

		[[nodiscard]] virtual ox::ValErr<uint8_t*> read(uint64_t inode) = 0;

		[[nodiscard]] ox::Error read(FileAddress addr, void *buffer, std::size_t size);

		[[nodiscard]] ox::Error read(FileAddress addr, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size);

		[[nodiscard]] ox::ValErr<uint8_t*> read(FileAddress addr);

		[[nodiscard]] virtual ox::Error remove(const char *path, bool recursive = false) = 0;

		[[nodiscard]] ox::Error remove(FileAddress addr, bool recursive = false);

		[[nodiscard]] virtual ox::Error resize(uint64_t size, void *buffer = nullptr) = 0;

		[[nodiscard]] virtual ox::Error write(const char *path, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) = 0;

		[[nodiscard]] virtual ox::Error write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) = 0;

		[[nodiscard]] ox::Error write(FileAddress addr, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile);

		[[nodiscard]] virtual ox::ValErr<FileStat> stat(uint64_t inode) = 0;

		[[nodiscard]] virtual ox::ValErr<FileStat> stat(const char *path) = 0;

		[[nodiscard]] ox::ValErr<FileStat> stat(FileAddress addr);

		[[nodiscard]] virtual uint64_t spaceNeeded(uint64_t size) = 0;

		[[nodiscard]] virtual uint64_t available() = 0;

		[[nodiscard]] virtual uint64_t size() const = 0;

		[[nodiscard]] virtual uint8_t *buff() = 0;

		[[nodiscard]] virtual ox::Error walk(ox::Error(*cb)(uint8_t, uint64_t, uint64_t)) = 0;

		[[nodiscard]] virtual bool valid() const = 0;

};

/**
 * FileSystemTemplate used to create file system that wraps around a FileStore,
 * taking an inode size and a directory type as parameters.
 *
 * Note: Directory parameter must have a default constructor.
 */
template<typename FileStore, typename Directory>
class FileSystemTemplate: public FileSystem {
	private:
		static constexpr auto InodeFsData = 2;

		struct __attribute__((packed)) FileSystemData {
			LittleEndian<typename FileStore::InodeId_t> rootDirInode;
		};

		FileStore m_fs;

	public:
		FileSystemTemplate() = default;

		FileSystemTemplate(FileStore fs);

		~FileSystemTemplate();

		[[nodiscard]] static ox::Error format(void *buff, uint64_t buffSize);

		[[nodiscard]] ox::Error mkdir(const char *path, bool recursive = false) override;

		[[nodiscard]] ox::Error move(const char *src, const char *dest) override;

		[[nodiscard]] ox::Error read(const char *path, void *buffer, std::size_t buffSize) override;

		[[nodiscard]] ox::ValErr<uint8_t*> read(const char*) override;

		[[nodiscard]] ox::Error read(uint64_t inode, void *buffer, std::size_t size) override;

		[[nodiscard]] ox::Error read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) override;

		[[nodiscard]] ox::ValErr<uint8_t*> read(uint64_t) override;

		template<typename F>
		[[nodiscard]] ox::Error ls(const char *dir, F cb);

		[[nodiscard]] ox::Error remove(const char *path, bool recursive = false) override;

		/**
		 * Resizes FileSystem to minimum possible size.
		 */
		[[nodiscard]] ox::Error resize();

		[[nodiscard]] ox::Error resize(uint64_t size, void *buffer = nullptr) override;

		[[nodiscard]] ox::Error write(const char *path, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) override;

		[[nodiscard]] ox::Error write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) override;

		[[nodiscard]] ox::ValErr<FileStat> stat(uint64_t inode) override;

		[[nodiscard]] ox::ValErr<FileStat> stat(const char *path) override;

		uint64_t spaceNeeded(uint64_t size) override;

		uint64_t available() override;

		uint64_t size() const override;

		uint8_t *buff() override;

		[[nodiscard]] ox::Error walk(ox::Error(*cb)(uint8_t, uint64_t, uint64_t)) override;

		bool valid() const override;

	private:
		[[nodiscard]] ValErr<FileSystemData> fileSystemData() const noexcept;

		/**
		 * Finds the inode ID at the given path.
		 */
		[[nodiscard]] ValErr<uint64_t> find(const char *path) const noexcept;

		[[nodiscard]] ValErr<Directory> rootDir() const noexcept;

};

template<typename FileStore, typename Directory>
FileSystemTemplate<FileStore, Directory>::FileSystemTemplate(FileStore fs) {
	m_fs = fs;
}

template<typename FileStore, typename Directory>
FileSystemTemplate<FileStore, Directory>::~FileSystemTemplate() {
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::format(void *buff, uint64_t buffSize) {
	oxReturnError(FileStore::format(buff, buffSize));
	FileStore fs(buff, buffSize);

	constexpr auto rootDirInode = MaxValue<typename FileStore::InodeId_t> / 2;
	Directory rootDir(fs, rootDirInode);
	oxReturnError(rootDir.init());

	FileSystemData fd;
	fd.rootDirInode = rootDirInode;
	oxTrace("ox::fs::FileSystemTemplate::format") << "rootDirInode:" << fd.rootDirInode;
	oxReturnError(fs.write(InodeFsData, &fd, sizeof(fd)));

	if (!fs.read(fd.rootDirInode).valid()) {
		oxTrace("ox::fs::FileSystemTemplate::format::error") << "FileSystemTemplate::format did not correctly create root directory";
		return OxError(1);
	}

	return OxError(0);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::mkdir(const char *path, bool recursive) {
	oxTrace("ox::fs::FileSystemTemplate::mkdir") << "path:" << path << "recursive:" << recursive;
	auto rootDir = this->rootDir();
	oxReturnError(rootDir.error);
	return rootDir.value.mkdir(path, recursive);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::move(const char *src, const char *dest) {
	auto fd = fileSystemData();
	oxReturnError(fd.error);
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto [inode, err] = rootDir.find(src);
	oxReturnError(err);
	oxReturnError(rootDir.write(dest, inode));
	oxReturnError(rootDir.remove(src));
	return OxError(0);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::read(const char *path, void *buffer, std::size_t buffSize) {
	auto fd = fileSystemData();
	oxReturnError(fd.error);
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto [inode, err] = rootDir.find(path);
	oxReturnError(err);
	return read(inode, buffer, buffSize);
}

template<typename FileStore, typename Directory>
[[nodiscard]] ox::ValErr<uint8_t*> FileSystemTemplate<FileStore, Directory>::read(const char *path) {
	auto fd = fileSystemData();
	oxReturnError(fd.error);
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto [inode, err] = rootDir.find(path);
	oxReturnError(err);
	return read(inode);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::read(uint64_t inode, void *buffer, std::size_t buffSize) {
	return m_fs.read(inode, buffer, buffSize);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) {
	return m_fs.read(inode, readStart, readSize, reinterpret_cast<uint8_t*>(buffer), size);
}

template<typename FileStore, typename Directory>
[[nodiscard]] ox::ValErr<uint8_t*> FileSystemTemplate<FileStore, Directory>::read(uint64_t inode) {
	auto data = m_fs.read(inode);
	if (!data.valid()) {
		return OxError(1);
	}
	return data.get();
}

template<typename FileStore, typename Directory>
template<typename F>
ox::Error FileSystemTemplate<FileStore, Directory>::ls(const char *path, F cb) {
	oxTrace("ox::FileSystemTemplate::ls") << "path:" << path;
	auto [s, err] = stat(path);
	oxReturnError(err);
	Directory dir(m_fs, s.inode);
	return dir.ls(cb);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::remove(const char *path, bool recursive) {
	auto fd = fileSystemData();
	oxReturnError(fd.error);
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto inode = rootDir.find(path);
	oxReturnError(inode.error);
	auto st = stat(inode.value);
	oxReturnError(st.error);
	if (st.value.fileType == FileType_NormalFile || recursive) {
		if (auto err = rootDir.remove(path)) {
			// removal failed, try putting the index back
			oxLogError(rootDir.write(path, inode.value));
			return err;
		}
	} else {
		oxTrace("FileSystemTemplate::remove::fail") << "Tried to remove directory without recursive setting.";
		return OxError(1);
	}
	return OxError(0);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::resize() {
	return m_fs.resize();
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::resize(uint64_t size, void *buffer) {
	oxReturnError(m_fs.resize(size, buffer));
	return OxError(0);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::write(const char *path, void *buffer, uint64_t size, uint8_t fileType) {
	auto [inode, err] = find(path);
	if (err) {
		auto generated = m_fs.generateInodeId();
		oxReturnError(generated.error);
		inode = generated.value;
	}
	auto rootDir = this->rootDir();
	oxReturnError(rootDir.error);
	oxReturnError(rootDir.value.write(path, inode));
	oxReturnError(write(inode, buffer, size, fileType));
	return OxError(0);
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) {
	return m_fs.write(inode, buffer, size, fileType);
}

template<typename FileStore, typename Directory>
ValErr<FileStat> FileSystemTemplate<FileStore, Directory>::stat(uint64_t inode) {
	auto s = m_fs.stat(inode);
	FileStat out;
	out.inode = s.value.inode;
	out.links = s.value.links;
	out.size = s.value.size;
	out.fileType = s.value.fileType;
	return {out, s.error};
}

template<typename FileStore, typename Directory>
ValErr<FileStat> FileSystemTemplate<FileStore, Directory>::stat(const char *path) {
	auto inode = find(path);
	if (inode.error) {
		return {{}, inode.error};
	}
	return stat(inode.value);
}

template<typename FileStore, typename Directory>
uint64_t FileSystemTemplate<FileStore, Directory>::spaceNeeded(uint64_t size) {
	return m_fs.spaceNeeded(size);
}

template<typename FileStore, typename Directory>
uint64_t FileSystemTemplate<FileStore, Directory>::available() {
	return m_fs.available();
}

template<typename FileStore, typename Directory>
uint64_t FileSystemTemplate<FileStore, Directory>::size() const {
	return m_fs.size();
}

template<typename FileStore, typename Directory>
uint8_t *FileSystemTemplate<FileStore, Directory>::buff() {
	return m_fs.buff();
}

template<typename FileStore, typename Directory>
ox::Error FileSystemTemplate<FileStore, Directory>::walk(ox::Error(*cb)(uint8_t, uint64_t, uint64_t)) {
	return m_fs.walk(cb);
}

template<typename FileStore, typename Directory>
bool FileSystemTemplate<FileStore, Directory>::valid() const {
	return m_fs.valid();
}

template<typename FileStore, typename Directory>
ValErr<typename FileSystemTemplate<FileStore, Directory>::FileSystemData> FileSystemTemplate<FileStore, Directory>::fileSystemData() const noexcept {
	FileSystemData fd;
	auto err = m_fs.read(InodeFsData, &fd, sizeof(fd));
	if (err != 0) {
		return {fd, err};
	}
	return fd;
}

template<typename FileStore, typename Directory>
ValErr<uint64_t> FileSystemTemplate<FileStore, Directory>::find(const char *path) const noexcept {
	auto fd = fileSystemData();
	if (fd.error) {
		return {0, fd.error};
	}
	// return root as a special case
	if (ox_strcmp(path, "/") == 0) {
		return static_cast<uint64_t>(fd.value.rootDirInode);
	}
	Directory rootDir(m_fs, fd.value.rootDirInode);
	auto inode = rootDir.find(path);
	if (inode.error) {
		return {0, inode.error};
	}
	return inode.value;
}

template<typename FileStore, typename Directory>
ValErr<Directory> FileSystemTemplate<FileStore, Directory>::rootDir() const noexcept {
	auto fd = fileSystemData();
	if (fd.error) {
		return {{}, fd.error};
	}
	return Directory(m_fs, fd.value.rootDirInode);
}

extern template class FileSystemTemplate<FileStore16, Directory16>;
extern template class FileSystemTemplate<FileStore32, Directory32>;

using FileSystem16 = FileSystemTemplate<FileStore16, Directory16>;
using FileSystem32 = FileSystemTemplate<FileStore32, Directory32>;

}
