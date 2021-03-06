/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/buffer.hpp>

#include <ox/fs/filestore/filestoretemplate.hpp>
#include <ox/fs/filesystem/filelocation.hpp>
#include <ox/fs/filesystem/types.hpp>

#include "directory.hpp"

namespace ox {

class FileSystem {

	public:
		virtual ~FileSystem() noexcept = default;

		virtual Error mkdir(const char *path, bool recursive) noexcept = 0;

		/**
		 * Moves an entry from one directory to another.
		 * @param src the path to the file
		 * @param dest the path of the destination directory
		 */
		virtual Error move(const char *src, const char *dest) noexcept = 0;

		virtual Error read(const char *path, void *buffer, std::size_t buffSize) noexcept = 0;

		virtual Result<const char*> directAccess(const char *path) noexcept = 0;

		virtual Error read(uint64_t inode, void *buffer, std::size_t size) noexcept = 0;

		virtual Error read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) noexcept = 0;

		virtual Result<const char*> directAccess(uint64_t inode) noexcept = 0;

		Error read(const FileAddress &addr, void *buffer, std::size_t size) noexcept;

		Result<Buffer> read(const FileAddress &addr) noexcept;

		Error read(const FileAddress &addr, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) noexcept;

		[[maybe_unused]]
		Result<const char*> directAccess(const FileAddress &addr) noexcept;

		Result<Vector<String>> ls(const String &dir) noexcept;

		virtual Result<Vector<String>> ls(const char *dir) noexcept = 0;

		virtual Error remove(const char *path, bool recursive) noexcept = 0;

		Error remove(const FileAddress &addr, bool recursive = false) noexcept;

		virtual Error resize(uint64_t size, void *buffer) noexcept = 0;

		virtual Error write(const char *path, const void *buffer, uint64_t size, FileType fileType) noexcept = 0;

		virtual Error write(uint64_t inode, const void *buffer, uint64_t size, FileType fileType) noexcept = 0;

		Error write(const FileAddress &addr, const void *buffer, uint64_t size, FileType fileType = FileType::NormalFile) noexcept;

		virtual Result<FileStat> stat(uint64_t inode) noexcept = 0;

		virtual Result<FileStat> stat(const char *path) noexcept = 0;

		Result<FileStat> stat(const FileAddress &addr) noexcept;

		[[nodiscard]]
		virtual uint64_t spaceNeeded(uint64_t size) noexcept = 0;

		virtual Result<uint64_t> available() noexcept = 0;

		virtual Result<uint64_t> size() const noexcept = 0;

		[[nodiscard]]
		virtual char *buff() noexcept = 0;

		virtual Error walk(Error(*cb)(uint8_t, uint64_t, uint64_t)) noexcept = 0;

		[[nodiscard]]
		virtual bool valid() const noexcept = 0;

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

		struct OX_PACKED FileSystemData {
			LittleEndian<typename FileStore::InodeId_t> rootDirInode;
		};

		FileStore m_fs;
		void(*m_freeBuffer)(char*) = nullptr;

	public:
		FileSystemTemplate() noexcept = default;

		FileSystemTemplate(void *buffer, uint64_t bufferSize, void(*freeBuffer)(char*) = [](const char *buff) { delete buff; }) noexcept;

		explicit FileSystemTemplate(FileStore fs) noexcept;

		~FileSystemTemplate() noexcept override;

		static Error format(void *buff, uint64_t buffSize) noexcept;

		Error mkdir(const char *path, bool recursive) noexcept override;

		Error move(const char *src, const char *dest) noexcept override;

		Error read(const char *path, void *buffer, std::size_t buffSize) noexcept override;

		Result<const char*> directAccess(const char*) noexcept override;

		Error read(uint64_t inode, void *buffer, std::size_t size) noexcept override;

		Error read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) noexcept override;

		Result<const char*> directAccess(uint64_t) noexcept override;

		Result<Vector<String>> ls(const char *path) noexcept override;

		template<typename F>
		Error ls(const char *path, F cb);

		Error remove(const char *path, bool recursive) noexcept override;

		/**
		 * Resizes FileSystem to minimum possible size.
		 */
		Error resize() noexcept;

		Error resize(uint64_t size, void *buffer) noexcept override;

		Error write(const char *path, const void *buffer, uint64_t size, FileType fileType) noexcept override;

		Error write(uint64_t inode, const void *buffer, uint64_t size, FileType fileType) noexcept override;

		Result<FileStat> stat(uint64_t inode) noexcept override;

		Result<FileStat> stat(const char *path) noexcept override;

		uint64_t spaceNeeded(uint64_t size) noexcept override;

		Result<uint64_t> available() noexcept override;

		Result<uint64_t> size() const noexcept override;

		char *buff() noexcept override;

		Error walk(Error(*cb)(uint8_t, uint64_t, uint64_t)) noexcept override;

		[[nodiscard]]
		bool valid() const noexcept override;

	private:
		Result<FileSystemData> fileSystemData() const noexcept;

		/**
		 * Finds the inode ID at the given path.
		 */
		Result<uint64_t> find(const char *path) const noexcept;

		Result<Directory> rootDir() const noexcept;

};

template<typename FileStore, typename Directory>
FileSystemTemplate<FileStore, Directory>::FileSystemTemplate(FileStore fs) noexcept {
	m_fs = fs;
}

template<typename FileStore, typename Directory>
FileSystemTemplate<FileStore, Directory>::FileSystemTemplate(void *buffer, uint64_t bufferSize, void(*freeBuffer)(char*)) noexcept:
	m_fs(buffer, bufferSize),
	m_freeBuffer(freeBuffer) {
}

template<typename FileStore, typename Directory>
FileSystemTemplate<FileStore, Directory>::~FileSystemTemplate() noexcept {
	if (m_freeBuffer && m_fs.buff()) {
		m_freeBuffer(m_fs.buff());
	}
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::format(void *buff, uint64_t buffSize) noexcept {
	oxReturnError(FileStore::format(buff, buffSize));
	FileStore fs(buff, buffSize);

	constexpr auto rootDirInode = MaxValue<typename FileStore::InodeId_t> / 2;
	Directory rootDir(fs, rootDirInode);
	oxReturnError(rootDir.init());

	FileSystemData fd;
	fd.rootDirInode = rootDirInode;
	oxTracef("ox::fs::FileSystemTemplate::format", "rootDirInode: {}", fd.rootDirInode.get());
	oxReturnError(fs.write(InodeFsData, &fd, sizeof(fd)));

	if (!fs.read(fd.rootDirInode).valid()) {
		oxTrace("ox::fs::FileSystemTemplate::format::error", "FileSystemTemplate::format did not correctly create root directory");
		return OxError(1);
	}

	return OxError(0);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::mkdir(const char *path, bool recursive) noexcept {
	oxTracef("ox::fs::FileSystemTemplate::mkdir", "path: {}, recursive: {}", path, recursive);
	oxRequireM(rootDir, this->rootDir());
	return rootDir.mkdir(path, recursive);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::move(const char *src, const char *dest) noexcept {
	oxRequire(fd, fileSystemData());
	Directory rootDir(m_fs, fd.rootDirInode);
	oxRequireM(inode, rootDir.find(src));
	oxReturnError(rootDir.write(dest, inode));
	oxReturnError(rootDir.remove(src));
	return OxError(0);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::read(const char *path, void *buffer, std::size_t buffSize) noexcept {
	oxRequire(fd, fileSystemData());
	Directory rootDir(m_fs, fd.rootDirInode);
	oxRequire(inode, rootDir.find(path));
	return read(inode, buffer, buffSize);
}

template<typename FileStore, typename Directory>
Result<const char*> FileSystemTemplate<FileStore, Directory>::directAccess(const char *path) noexcept {
	oxRequire(fd, fileSystemData());
	Directory rootDir(m_fs, fd.rootDirInode);
	oxRequire(inode, rootDir.find(path));
	return directAccess(inode);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::read(uint64_t inode, void *buffer, std::size_t buffSize) noexcept {
	return m_fs.read(inode, buffer, buffSize);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) noexcept {
	return m_fs.read(inode, readStart, readSize, reinterpret_cast<uint8_t*>(buffer), size);
}

template<typename FileStore, typename Directory>
Result<const char*> FileSystemTemplate<FileStore, Directory>::directAccess(uint64_t inode) noexcept {
	auto data = m_fs.read(inode);
	if (!data.valid()) {
		return OxError(1);
	}
	return bit_cast<char*>(data.get());
}

template<typename FileStore, typename Directory>
Result<Vector<String>> FileSystemTemplate<FileStore, Directory>::ls(const char *path) noexcept {
	Vector<String> out;
	oxReturnError(ls(path, [&out](const char *name, typename FileStore::InodeId_t) {
		out.emplace_back(name);
		return OxError(0);
	}));
	return ox::move(out);
}

template<typename FileStore, typename Directory>
template<typename F>
Error FileSystemTemplate<FileStore, Directory>::ls(const char *path, F cb) {
	oxTracef("ox::fs::FileSystemTemplate::ls", "path: {}", path);
	oxRequire(s, stat(path));
	Directory dir(m_fs, s.inode);
	return dir.ls(cb);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::remove(const char *path, bool recursive) noexcept {
	oxRequire(fd, fileSystemData());
	Directory rootDir(m_fs, fd.rootDirInode);
	oxRequire(inode, rootDir.find(path));
	oxRequire(st, stat(inode));
	if (st.fileType == FileType::NormalFile || recursive) {
		if (auto err = rootDir.remove(path)) {
			// removal failed, try putting the index back
			oxLogError(rootDir.write(path, inode));
			return err;
		}
	} else {
		oxTrace("FileSystemTemplate::remove::fail", "Tried to remove directory without recursive setting.");
		return OxError(1);
	}
	return OxError(0);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::resize() noexcept {
	return m_fs.resize();
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::resize(uint64_t size, void *buffer) noexcept {
	oxReturnError(m_fs.resize(size, buffer));
	return OxError(0);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::write(const char *path, const void *buffer, uint64_t size, FileType fileType) noexcept {
	auto [inode, err] = find(path);
	if (err) {
		oxRequire(generatedId, m_fs.generateInodeId());
		inode = generatedId;
	}
	oxRequireM(rootDir, this->rootDir());
	oxReturnError(rootDir.write(path, inode));
	oxReturnError(write(inode, buffer, size, fileType));
	return OxError(0);
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::write(uint64_t inode, const void *buffer, uint64_t size, FileType fileType) noexcept {
	return m_fs.write(inode, buffer, size, static_cast<uint8_t>(fileType));
}

template<typename FileStore, typename Directory>
Result<FileStat> FileSystemTemplate<FileStore, Directory>::stat(uint64_t inode) noexcept {
	oxRequire(s, m_fs.stat(inode));
	FileStat out;
	out.inode = s.inode;
	out.links = s.links;
	out.size = s.size;
	out.fileType = static_cast<FileType>(s.fileType);
	return out;
}

template<typename FileStore, typename Directory>
Result<FileStat> FileSystemTemplate<FileStore, Directory>::stat(const char *path) noexcept {
	oxRequire(inode, find(path));
	return stat(inode);
}

template<typename FileStore, typename Directory>
uint64_t FileSystemTemplate<FileStore, Directory>::spaceNeeded(uint64_t size) noexcept {
	return m_fs.spaceNeeded(size);
}

template<typename FileStore, typename Directory>
Result<uint64_t> FileSystemTemplate<FileStore, Directory>::available() noexcept {
	return m_fs.available();
}

template<typename FileStore, typename Directory>
Result<uint64_t> FileSystemTemplate<FileStore, Directory>::size() const noexcept {
	return m_fs.size();
}

template<typename FileStore, typename Directory>
char *FileSystemTemplate<FileStore, Directory>::buff() noexcept {
	return m_fs.buff();
}

template<typename FileStore, typename Directory>
Error FileSystemTemplate<FileStore, Directory>::walk(Error(*cb)(uint8_t, uint64_t, uint64_t)) noexcept {
	return m_fs.walk(cb);
}

template<typename FileStore, typename Directory>
bool FileSystemTemplate<FileStore, Directory>::valid() const noexcept {
	return m_fs.valid();
}

template<typename FileStore, typename Directory>
Result<typename FileSystemTemplate<FileStore, Directory>::FileSystemData> FileSystemTemplate<FileStore, Directory>::fileSystemData() const noexcept {
	FileSystemData fd;
	oxReturnError(m_fs.read(InodeFsData, &fd, sizeof(fd)));
	return fd;
}

template<typename FileStore, typename Directory>
Result<uint64_t> FileSystemTemplate<FileStore, Directory>::find(const char *path) const noexcept {
	oxRequire(fd, fileSystemData());
	// return root as a special case
	if (ox_strcmp(path, "/") == 0) {
		return static_cast<uint64_t>(fd.rootDirInode);
	}
	Directory rootDir(m_fs, fd.rootDirInode);
	oxRequire(out, rootDir.find(path));
	return static_cast<uint64_t>(out);
}

template<typename FileStore, typename Directory>
Result<Directory> FileSystemTemplate<FileStore, Directory>::rootDir() const noexcept {
	oxRequire(fd, fileSystemData());
	return Directory(m_fs, fd.rootDirInode);
}

extern template class FileSystemTemplate<FileStore16, Directory16>;
extern template class FileSystemTemplate<FileStore32, Directory32>;

using FileSystem16 = FileSystemTemplate<FileStore16, Directory16>;
using FileSystem32 = FileSystemTemplate<FileStore32, Directory32>;

}
