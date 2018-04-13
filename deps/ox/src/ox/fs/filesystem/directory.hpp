/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>

#include "types.hpp"

namespace ox {

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

	public:
		InodeId_t inode = 0;

	private:
		uint32_t m_nameLen = 0;

	public:
		void *end() {
			return ((uint8_t*) this) + size();
		}

		char *getName() {
			return (char*) (this + 1);
		}

		void setName(const char *name) {
			auto data = getName();
			m_nameLen = ox_strlen(name);
			ox_memcpy(data, name, m_nameLen);
			data[m_nameLen] = 0;
		}

		static uint64_t spaceNeeded(const char *fileName) {
			return sizeof(DirectoryEntry) + ox_strlen(fileName) + 1;
		}

		/**
		 * The size in bytes.
		 */
		uint64_t size() {
			return sizeof(DirectoryEntry) + m_nameLen + 1;
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

	void *end();

	uint64_t getFileInode(const char *name);

	int getChildrenInodes(InodeId_t *inodes, std::size_t inodesLen);

	int rmFile(const char *name);

	int copy(Directory<uint64_t, uint64_t> *dirOut);

	template<typename List>
	int ls(List *list);
};

template<typename InodeId_t, typename FsSize_t>
void *Directory<InodeId_t, FsSize_t>::end() {
	return ((int8_t*) this) + size;
}

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
int Directory<InodeId_t, FsSize_t>::getChildrenInodes(InodeId_t *inodes, std::size_t inodesLen) {
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
			if (this->end() < current->end()) {
				return 1;
			}
			entry->inode = current->inode;
			entry->setName(current->getName());

			current = (DirectoryEntry<InodeId_t>*) (((uint8_t*) current) + current->size());
			dirOutBuff += entry->size();
		}
	} else {
		return 2;
	}
	return 0;
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

}
