/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <ox/std/std.hpp>
#include "pathiterator.hpp"
#include "filestore.hpp"

namespace ox {

enum FsType {
	OxFS_16 = 1,
	OxFS_32 = 2,
	OxFS_64 = 3
};

enum FileType {
	FileType_NormalFile = 1,
	FileType_Directory  = 2
};

struct FileStat {
	uint64_t inode;
	uint64_t links;
	uint64_t size;
	uint8_t  fileType;
};

template<typename String>
struct DirectoryListing {
	String name;
	FileStat stat;

	DirectoryListing() = default;

	DirectoryListing(const char *name) {
		this->name = name;
	}
};

template<typename String>
bool operator<(const DirectoryListing<String> &a, const DirectoryListing<String> &b) {
	return a.name < b.name;
}

template<typename InodeId_t>
struct __attribute__((packed)) DirectoryEntry {
	InodeId_t inode;

	char *getName() {
		return (char*) (this + 1);
	}

	void setName(const char *name) {
		auto data = getName();
		auto nameLen = ox_strlen(name);
		ox_memcpy(data, name, nameLen);
		data[nameLen] = 0;
	}

	static uint64_t spaceNeeded(const char *fileName) {
		return sizeof(DirectoryEntry) + ox_strlen(fileName) + 1;
	}

	/**
	 * The size in bytes.
	 */
	uint64_t size() {
		return spaceNeeded(getName());
	}
};

template<typename InodeId_t, typename FsSize_t>
struct __attribute__((packed)) Directory {
	/**
	 * Number of bytes after this Directory struct.
	 */
	FsSize_t size = 0;
	FsSize_t children = 0;

	DirectoryEntry<InodeId_t> *files() {
		return size ? (DirectoryEntry<InodeId_t>*) (this + 1) : nullptr;
	}

	uint64_t getFileInode(const char *name);

	int getChildrenInodes(InodeId_t *inodes, size_t inodesLen);

	int rmFile(const char *name);

	int copy(Directory<uint64_t, uint64_t> *dirOut);

	template<typename List>
	int ls(List *list);
};

template<typename InodeId_t, typename FsSize_t>
uint64_t Directory<InodeId_t, FsSize_t>::getFileInode(const char *name) {
	uint64_t inode = 0;
	auto current = files();
	if (current) {
		for (uint64_t i = 0; ox_strcmp(current->getName(), name) != 0;) {
			i += current->size();
			if (i < this->size) {
				current = (DirectoryEntry<InodeId_t>*) (((uint8_t*) current) + current->size());
			} else {
				current = nullptr;
				break;
			}
		}
		if (current) {
			inode = current->inode;
		}
	}
	return inode;
}

template<typename InodeId_t, typename FsSize_t>
int Directory<InodeId_t, FsSize_t>::getChildrenInodes(InodeId_t *inodes, size_t inodesLen) {
	if (inodesLen >= this->children) {
		auto current = files();
		if (current) {
			for (uint64_t i = 0; i < this->children; i++) {
				if (ox_strcmp(current->getName(), ".") and ox_strcmp(current->getName(), "..")) {
					inodes[i] = current->inode;
				}
				current = (DirectoryEntry<InodeId_t>*) (((uint8_t*) current) + current->size());
			}
			return 0;
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}

template<typename InodeId_t, typename FsSize_t>
int Directory<InodeId_t, FsSize_t>::rmFile(const char *name) {
	int err = 1;
	auto current = files();
	if (current) {
		for (uint64_t i = 0; i < this->size;) {
			i += current->size();
			if (ox_strcmp(current->getName(), name) == 0) {
				auto dest = (uint8_t*) current;
				auto src = dest + current->size();
				ox_memcpy(dest, src, this->size - i);
				this->size -= current->size();
				this->children--;
				err = 0;
				break;
			}
			current = (DirectoryEntry<InodeId_t>*) (((uint8_t*) current) + current->size());
		}
	}
	return err;
}

template<typename InodeId_t, typename FsSize_t>
int Directory<InodeId_t, FsSize_t>::copy(Directory<uint64_t, uint64_t> *dirOut) {
	auto current = files();
	auto dirOutBuff = (uint8_t*) dirOut;
	dirOutBuff += sizeof(Directory<uint64_t, uint64_t>);
	dirOut->size = this->size;
	dirOut->children = this->children;
	if (current) {
		for (uint64_t i = 0; i < this->children; i++) {
			auto entry = (DirectoryEntry<uint64_t>*) dirOutBuff;
			entry->inode = current->inode;
			entry->setName(current->getName());

			current = (DirectoryEntry<InodeId_t>*) (((uint8_t*) current) + current->size());
			dirOutBuff += entry->size();
		}
		return 0;
	} else {
		return 1;
	}
}

template<typename InodeId_t, typename FsSize_t>
template<typename List>
int Directory<InodeId_t, FsSize_t>::ls(List *list) {
	auto current = files();
	if (current) {
		for (uint64_t i = 0; i < this->children; i++) {
			list->push_back(current->getName());
			(*list)[i].stat.inode = current->inode;
			current = (DirectoryEntry<InodeId_t>*) (((uint8_t*) current) + current->size());
		}
		return 0;
	} else {
		return 1;
	}
}


class FileSystem {
	public:
		virtual ~FileSystem() {};

		virtual int stripDirectories() = 0;

		virtual int mkdir(const char *path) = 0;

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
	int err = 0;
	auto s = stat(path);
	if (s.fileType == FileType_Directory) {
		uint8_t dirBuff[s.size * 4];
		auto dir = (Directory<uint64_t, uint64_t>*) dirBuff;
		err = readDirectory(path, dir);
		err |= dir->ls(list);
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

template<typename FileStore, FsType FS_TYPE>
class FileSystemTemplate: public FileSystem {

	private:
		FileStore *m_store = nullptr;
		bool m_ownsBuff = false;

	public:
		// static members
		static typename FileStore::InodeId_t INODE_RANDOM;
		static typename FileStore::InodeId_t INODE_ROOT_DIR;
		static typename FileStore::InodeId_t INODE_RESERVED_END;

		explicit FileSystemTemplate(uint8_t *buff, bool ownsBuff = false);

		~FileSystemTemplate();

		int stripDirectories() override;

		int mkdir(const char *path) override;

		int read(const char *path, void *buffer, size_t buffSize) override;

		int read(uint64_t inode, void *buffer, size_t buffSize) override;

		int read(uint64_t inode, size_t readStart, size_t readSize, void *buffer, size_t *size) override;

		uint8_t *read(uint64_t inode, size_t *size) override;

		void resize(uint64_t size = 0) override;

		int remove(uint64_t inode, bool recursive = false) override;

		int remove(const char *path, bool recursive = false) override;

		int write(const char *path, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) override;

		int write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) override;

		FileStat stat(const char *path) override;

		FileStat stat(uint64_t inode) override;

		uint64_t findInodeOf(const char *name);

		uint64_t spaceNeeded(uint64_t size) override;

		uint64_t available() override;

		uint64_t size() override;

		uint8_t *buff() override;

		int move(const char *src, const char *dest) override;

		/**
		 * Removes an entry from a directory. This does not delete the referred to file.
		 */
		int rmDirectoryEntry(const char *path);

		void walk(int(*cb)(const char*, uint64_t, uint64_t)) override;

		static uint8_t *format(uint8_t *buffer, typename FileStore::FsSize_t size, bool useDirectories);

	protected:
		int readDirectory(const char *path, Directory<uint64_t, uint64_t> *dirOut) override;

	private:
		uint64_t generateInodeId();

		int insertDirectoryEntry(const char *dirPath, const char *fileName, uint64_t inode);

		void expand(uint64_t size);
};

template<typename FileStore, FsType FS_TYPE>
FileSystemTemplate<FileStore, FS_TYPE>::FileSystemTemplate(uint8_t *buff, bool ownsBuff) {
	m_store = (FileStore*) buff;
	m_ownsBuff = ownsBuff;
}

template<typename FileStore, FsType FS_TYPE>
FileSystemTemplate<FileStore, FS_TYPE>::~FileSystemTemplate() {
	if (m_ownsBuff) {
		delete[] (uint8_t*) m_store;
	}
}

template<typename FileStore, FsType FS_TYPE>
typename FileStore::InodeId_t FileSystemTemplate<FileStore, FS_TYPE>::INODE_RANDOM = 1;

template<typename FileStore, FsType FS_TYPE>
typename FileStore::InodeId_t FileSystemTemplate<FileStore, FS_TYPE>::INODE_ROOT_DIR = 2;

template<typename FileStore, FsType FS_TYPE>
typename FileStore::InodeId_t FileSystemTemplate<FileStore, FS_TYPE>::INODE_RESERVED_END = 100;

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::stripDirectories() {
	return m_store->removeAllType(FileType::FileType_Directory);
}

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::mkdir(const char *pathIn) {
	if (!findInodeOf(pathIn)) {
		auto pathLen = ox_strlen(pathIn);
		char path[pathLen + 1];
		ox_memcpy(path, pathIn, pathLen + 1);

		// make sure last character does not end with /
		if (pathLen >= 1 && path[pathLen - 1] == '/') {
			path[pathLen - 1] = 0;
			pathLen--;
		}

		Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t> dir;
		auto err = write(path, &dir, sizeof(dir), FileType::FileType_Directory);
		if (err) {
			return err;
		}

		// add . entry for self
		auto inode = findInodeOf(path);
		err = insertDirectoryEntry(path, ".", inode);
		if (err) {
			remove(inode);
			return err;
		}

		// add .. entry for parent
		char dirPath[pathLen];
		PathIterator pathReader(path, pathLen);
		err |= pathReader.dirPath(dirPath, pathLen);
		err = insertDirectoryEntry(path, "..", findInodeOf(dirPath));
		if (err) {
			remove(inode);
			return err;
		}

		return err;
	} else {
		return 1;
	}
}

template<typename FileStore, FsType FS_TYPE>
FileStat FileSystemTemplate<FileStore, FS_TYPE>::stat(const char *path) {
	auto inode = findInodeOf(path);
	FileStat stat;
	auto s = m_store->stat(inode);
	stat.size = s.size;
	stat.inode = s.inodeId;
	stat.fileType = s.fileType;
	return stat;
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
FileStat FileSystemTemplate<FileStore, FS_TYPE>::stat(uint64_t inode) {
	FileStat stat;
	auto s = m_store->stat(inode);
	stat.size = s.size;
	stat.inode = s.inodeId;
	stat.links = s.links;
	stat.fileType = s.fileType;
	return stat;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::read(const char *path, void *buffer, size_t buffSize) {
	int retval = -1;

	// find the inode for the given path
	auto inode = findInodeOf(path);

	// if inode exists, read the data into buffer
	if (inode) {
		retval = read(inode, buffer, buffSize);
	}

	return retval;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, void *buffer, size_t buffSize) {
	auto stat = m_store->stat(inode);
	if (stat.size <= buffSize) {
		return m_store->read(inode, buffer, nullptr);
	}
	return -1;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, size_t readStart,
                                                 size_t readSize, void *buffer,
                                                 size_t *size) {
	if (size) {
		auto stat = m_store->stat(inode);
		*size = stat.size;
	}
	return m_store->read(inode, readStart, readSize, buffer, nullptr);
}
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, size_t *size) {
	auto s = m_store->stat(inode);
	auto buff = new uint8_t[s.size];
	if (size) {
		*size = s.size;
	}
	if (m_store->read(inode, buff, nullptr)) {
		delete []buff;
		buff = nullptr;
	}
	return buff;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::remove(const char *path, bool recursive) {
	auto inode = findInodeOf(path);
	if (inode) {
		return rmDirectoryEntry(path) | remove(inode, recursive);
	} else {
		return 1;
	}
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::remove(uint64_t inode, bool recursive) {
	auto fileType = stat(inode).fileType;
	if (fileType != FileType::FileType_Directory) {
		return m_store->remove(inode);
	} else if (fileType == FileType::FileType_Directory && recursive) {
		int err = 0;
		auto dirStat = stat(inode);
		auto dirBuffLen = dirStat.size;
		uint8_t dirBuff[dirBuffLen];
		auto dir = (Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t>*) dirBuff;

		err = read(dirStat.inode, dirBuff, dirBuffLen);
		if (err) {
			return 1;
		}

		typename FileStore::InodeId_t inodes[dir->children];
		ox_memset(inodes, 0, sizeof(typename FileStore::InodeId_t) * dir->children);
		dir->getChildrenInodes(inodes, dir->children);

		for (auto i : inodes) {
			if (i) {
				err |= remove(i, true);
			}
		}

		if (!err) {
			err |= m_store->remove(inode);
		}

		return err;
	} else {
		return 1;
	}
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::write(const char *path, void *buffer, uint64_t size, uint8_t fileType) {
	int err = 0;
	size_t pathLen = ox_strlen(path);
	char dirPath[pathLen];
	char fileName[pathLen];
	PathIterator pathReader(path, pathLen);
	err |= pathReader.fileName(fileName, pathLen);
	err |= pathReader.dirPath(dirPath, pathLen);
	if (err) {
		return err;
	}

	uint64_t inode = findInodeOf(path);
	// find an inode value for the given path
	if (!inode) {
		inode = generateInodeId();
		err |= write(inode, buffer, 0, fileType); // ensure file exists before indexing it
		err |= insertDirectoryEntry(dirPath, fileName, inode);
	}

	if (!err) {
		err = write(inode, buffer, size, fileType);
	}

	return err;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) {
	if (m_ownsBuff) {
		while (m_store->spaceNeeded(size) > m_store->available()) {
			expand(this->size() * 2);
		}
	}
	return m_store->write(inode, buffer, size, fileType);
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::findInodeOf(const char *path) {
	const auto pathLen = ox_strlen(path);
	PathIterator it(path, pathLen);
	char fileName[pathLen];
	uint64_t inode = INODE_ROOT_DIR;
	while (it.hasNext() && it.next(fileName, pathLen) == 0 && ox_strlen(fileName)) {
		auto dirStat = stat(inode);
		if (dirStat.inode && dirStat.size >= sizeof(Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t>)) {
			uint8_t dirBuffer[dirStat.size];
			auto dir = (Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t>*) dirBuffer;
			if (read(inode, dirBuffer, dirStat.size) == 0) {
				if (dirStat.fileType == FileType::FileType_Directory) {
					inode = dir->getFileInode(fileName);
				} else {
					inode = 0; // null out inode and break
					break;
				}
			} else {
				inode = 0; // null out inode and break
				break;
			}
		} else {
			inode = 0; // null out inode and break
			break;
		}
	}
	return inode;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

template<typename FileStore, FsType FS_TYPE>
void FileSystemTemplate<FileStore, FS_TYPE>::resize(uint64_t size) {
	return m_store->resize(size);
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::spaceNeeded(uint64_t size) {
	return m_store->spaceNeeded(size);
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::available() {
	return m_store->available();
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::size() {
	return m_store->size();
}

template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::buff() {
	return (uint8_t*) m_store;
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::format(uint8_t *buffer, typename FileStore::FsSize_t size, bool useDirectories) {
	buffer = FileStore::format(buffer, size, (uint16_t) FS_TYPE);

	if (buffer && useDirectories) {
		Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t> dir;
		FileSystemTemplate<FileStore, FS_TYPE> fs((uint8_t*) buffer);
		fs.write(INODE_ROOT_DIR, &dir, sizeof(dir), FileType::FileType_Directory);
	}

	return (uint8_t*) buffer;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::generateInodeId() {
	Random rand;
	read(INODE_RANDOM, &rand, sizeof(rand));

	uint64_t inode = 0;
	// find an inode value for the given path
	while (!inode) {
		inode = rand.gen();
		inode >>= 64 -  8 * sizeof(typename FileStore::InodeId_t);

		// make sure this does not already exist
		if (inode < INODE_RESERVED_END || stat(inode).inode) {
			// that result was unusable, try again
			inode = 0;
		}
	}

	write(INODE_RANDOM, &rand, sizeof(rand));

	return inode;
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::insertDirectoryEntry(const char *dirPath, const char *fileName, uint64_t inode) {
	auto s = stat(dirPath);
	if (s.inode) {
		auto spaceNeeded = DirectoryEntry<typename FileStore::InodeId_t>::spaceNeeded(fileName);
		size_t dirBuffSize = s.size + spaceNeeded;
		uint8_t dirBuff[dirBuffSize];
		int err = read(s.inode, dirBuff, dirBuffSize);

		if (!err) {
			auto dir = (Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t>*) dirBuff;
			dir->size += spaceNeeded;
			dir->children++;
			auto entry = (DirectoryEntry<typename FileStore::InodeId_t>*) &dirBuff[s.size];
			entry->inode = inode;
			entry->setName(fileName);
			err = write(s.inode, dirBuff, dirBuffSize, FileType_Directory);
			err |= m_store->incLinks(inode);
			return err;
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::move(const char *src, const char *dest) {
	auto inode = stat(src).inode;
	if (inode && !stat(dest).inode) {
		int err = 0;

		size_t srcLen = ox_strlen(src);
		char srcDirPath[srcLen];
		char srcFileName[srcLen];
		PathIterator srcPathReader(src, srcLen);
		err |= srcPathReader.fileName(srcFileName, srcLen);
		err |= srcPathReader.dirPath(srcDirPath, srcLen);
		if (err) {
			return err;
		}

		size_t destLen = ox_strlen(dest);
		char destDirPath[destLen];
		char destFileName[destLen];
		PathIterator destPathReader(dest, destLen);
		err |= destPathReader.fileName(destFileName, destLen);
		err |= destPathReader.dirPath(destDirPath, destLen);
		if (err) {
			return err;
		}

		err = rmDirectoryEntry(src);
		if (err) {
			return err;
		}

		err = insertDirectoryEntry(destDirPath, destFileName, inode);
		if (!err) {
			return err;
		}

		return 0;
	} else {
		return 1;
	}
}

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::rmDirectoryEntry(const char *path) {
	int err = 0;
	size_t pathLen = ox_strlen(path);
	char dirPath[pathLen];
	char fileName[pathLen];
	PathIterator pathReader(path, pathLen);
	err |= pathReader.fileName(fileName, pathLen);
	err |= pathReader.dirPath(dirPath, pathLen);
	if (err) {
		return err;
	}

	auto dirStat = stat(dirPath);
	auto dirBuffLen = dirStat.size;
	uint8_t dirBuff[dirBuffLen];

	err = read(dirStat.inode, dirBuff, dirBuffLen);
	if (err) {
		return err;
	}

	auto dir = (Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t>*) dirBuff;
	auto inode = dir->getFileInode(fileName);
	err |= dir->rmFile(fileName);
	err |= m_store->decLinks(inode);

	if (err) {
		return err;
	}

	err = write(dirStat.inode, dirBuff, dirBuffLen - DirectoryEntry<typename FileStore::InodeId_t>::spaceNeeded(fileName));

	return err;
}

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::readDirectory(const char *path, Directory<uint64_t, uint64_t> *dirOut) {
	int err = 0;
	auto inode = findInodeOf(path);
	auto dirStat = stat(inode);
	auto dirBuffLen = dirStat.size;
	uint8_t dirBuff[dirBuffLen];
	auto dir = (Directory<typename FileStore::InodeId_t, typename FileStore::FsSize_t>*) dirBuff;

	err = read(dirStat.inode, dirBuff, dirBuffLen);
	if (!err) {
		return dir->copy(dirOut);
	} else {
		return 1;
	}
}

template<typename FileStore, FsType FS_TYPE>
void FileSystemTemplate<FileStore, FS_TYPE>::expand(uint64_t newSize) {
	if (newSize > size()) {
		auto newBuff = new uint8_t[newSize];
		ox_memcpy(newBuff, m_store, m_store->size());
		delete[] m_store;
		m_store = (FileStore*) newBuff;
		resize(newSize);
	}
}

template<typename FileStore, FsType FS_TYPE>
void FileSystemTemplate<FileStore, FS_TYPE>::walk(int(*cb)(const char*, uint64_t, uint64_t)) {
	m_store->walk(cb);
}

typedef FileSystemTemplate<FileStore16, OxFS_16> FileSystem16;
typedef FileSystemTemplate<FileStore32, OxFS_32> FileSystem32;
typedef FileSystemTemplate<FileStore64, OxFS_64> FileSystem64;

}
