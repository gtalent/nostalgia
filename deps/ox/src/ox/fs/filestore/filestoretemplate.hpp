/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "filestore.hpp"
#include "nodebuffer.hpp"

namespace ox::fs {

template<typename size_t>
struct __attribute__((packed)) FileStoreItem: public Item<size_t> {
	ox::LittleEndian<size_t> id = 0;
	ox::LittleEndian<size_t> fileType = 0;
	ox::LittleEndian<size_t> links = 0;
	ox::LittleEndian<size_t> left = 0;
	ox::LittleEndian<size_t> right = 0;

	explicit FileStoreItem(size_t size): Item<size_t>(size) {
	}

	ox::fs::Ptr<uint8_t, size_t> data() {
		return Ptr<uint8_t, size_t>(this, this->size(), sizeof(*this), this->size() - sizeof(*this));
	}
};

template<typename size_t>
class FileStoreTemplate: public FileStore {

	private:
		using ItemPtr = typename ox::fs::NodeBuffer<size_t, FileStoreItem<uint32_t>>::ItemPtr;

		struct __attribute__((packed)) FileStoreData {
			ox::LittleEndian<size_t> rootNode = sizeof(NodeBuffer<size_t, FileStoreItem<uint32_t>>);
		};

		size_t m_buffSize = 0;
		ox::fs::NodeBuffer<size_t, FileStoreItem<uint32_t>> *m_buffer = nullptr;

	public:
		FileStoreTemplate(void *buff, size_t buffSize);

		Error format();

		Error setSize(InodeId_t buffSize);

		Error incLinks(InodeId_t id);

		Error decLinks(InodeId_t id);

		Error write(InodeId_t id, void *data, InodeId_t dataLen, uint8_t fileType = 0);

		Error read(InodeId_t id, void *data, InodeId_t *size);

		Error read(InodeId_t id, InodeId_t readStart, InodeId_t readSize, void *data, InodeId_t *size);

		StatInfo stat(InodeId_t id);

		InodeId_t spaceNeeded(InodeId_t size);

		InodeId_t size();

		InodeId_t available();

	private:
		FileStoreData &fileStoreData();

		/**
		 * Places the given Item at the given ID. If it already exists, the
		 * existing value will be overwritten.
		 */
		void placeItem(ItemPtr root, ItemPtr item);

		/**
		 * Finds the parent an inode by its ID.
		 */
		ItemPtr findParent(ItemPtr ptr, size_t id);

		/**
		 * Finds an inode by its ID.
		 */
		ItemPtr find(ItemPtr ptr, size_t id);

		/**
		 * Finds an inode by its ID.
		 */
		ItemPtr find(size_t id);

		/**
		 * Gets the root inode.
		 */
		ItemPtr rootInode();

		bool canWrite(InodeId_t id, size_t size);

};

template<typename size_t>
FileStoreTemplate<size_t>::FileStoreTemplate(void *buff, size_t buffSize) {
	m_buffSize = buffSize;
	m_buffer = static_cast<ox::fs::NodeBuffer<size_t, FileStoreItem<uint32_t>>*>(buff);
	if (!m_buffer->valid(buffSize)) {
		m_buffSize = 0;
		m_buffer = nullptr;
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::format() {
	new (m_buffer) NodeBuffer<size_t, FileStoreItem<uint32_t>>(m_buffSize);
	auto data = m_buffer->malloc(sizeof(FileStoreData));
	if (data.valid()) {
		new (data->data()) FileStoreData;
		return 0;
	}
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::setSize(InodeId_t size) {
	if (m_buffSize >= size) {
		return m_buffer->setSize(size);
	}
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::incLinks(InodeId_t id) {
	auto item = find(id);
	if (item.valid()) {
		item->links++;
		return 0;
	}
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::decLinks(InodeId_t id) {
	auto item = find(id);
	if (item.valid()) {
		item->links--;
		return 0;
	}
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::write(InodeId_t id, void *data, InodeId_t dataSize, uint8_t fileType) {
	// TODO: delete the old node if it exists
	if (canWrite(id, dataSize)) {
		auto dest = m_buffer->malloc(dataSize);
		// if first malloc failed, compact and try again
		dest = m_buffer->malloc(dataSize);
		if (dest.valid()) {
			new (dest) FileStoreItem(dataSize);
			dest->id = id;
			dest->fileType = fileType;
			ox_memcpy(dest->data(), data, dest->size());
			auto root = rootInode();
			if (root.valid()) {
				placeItem(root, dest);
			} else {
				fileStoreData().rootNode = root;
			}
			return 0;
		}
	}
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, void *data, InodeId_t *size) {
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, InodeId_t readStart, InodeId_t readSize, void *data, InodeId_t *size) {
	return 1;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::StatInfo FileStoreTemplate<size_t>::stat(InodeId_t id) {
	return {};
}

template<typename size_t>
InodeId_t FileStoreTemplate<size_t>::spaceNeeded(InodeId_t size) {
	return 1;
}

template<typename size_t>
InodeId_t FileStoreTemplate<size_t>::size() {
	return m_buffer->size();
}

template<typename size_t>
InodeId_t FileStoreTemplate<size_t>::available() {
	return m_buffer->available();
}

template<typename size_t>
typename FileStoreTemplate<size_t>::FileStoreData &FileStoreTemplate<size_t>::fileStoreData() {
	return *reinterpret_cast<FileStoreData*>(m_buffer->firstItem()->data().get());
}

template<typename size_t>
void FileStoreTemplate<size_t>::placeItem(ItemPtr root, ItemPtr item) {
	if (item->id > root->id) {
		auto right = m_buffer->ptr(root->right);
		if (right.valid()) {
			placeItem(right, item);
		} else {
			root->right = root;
		}
	} else if (item->id < root->id) {
		auto left = m_buffer->ptr(root->left);
		if (left.valid()) {
			placeItem(left, item);
		} else {
			root->left = root;
		}
	}
}

template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::findParent(ItemPtr item, size_t id) {
	if (id > item->id) {
		auto right = m_buffer->ptr(item->right);
		if (right.valid()) {
			if (right->id == id) {
				return item;
			} else {
				return findParent(right, id);
			}
		}
	} else if (id < item->id) {
		auto left = m_buffer->ptr(item->left);
		if (left.valid()) {
			if (left->id == id) {
				return item;
			} else {
				return findParent(left, id);
			}
		}
	}
	return nullptr;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::find(ItemPtr item, size_t id) {
	if (item.valid()) {
		if (id > item->id) {
			return find(m_buffer->ptr(item->right), id);
		} else if (id < item->id) {
			return find(m_buffer->ptr(item->left), id);
		} else {
			return item;
		}
	}
	return nullptr;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::find(size_t id) {
	auto root = m_buffer->ptr(fileStoreData().rootNode);
	if (root.valid()) {
		auto item = find(root, id);
		return item;
	}
	return nullptr;
}

/**
 * Gets the root inode.
 */
template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::rootInode() {
	return m_buffer->ptr(fileStoreData().rootNode);
}

template<typename size_t>
bool FileStoreTemplate<size_t>::canWrite(InodeId_t id, size_t size) {
	if (m_buffer->spaceNeeded(size) >= m_buffer->available()) {
		return true;
	} else {
		auto existing = find(id);
		if (existing.valid()) {
			if (m_buffer->spaceNeeded(size) >= existing.size()) {
				return true;
			}
		}
	}
	return false;
}

using FileStore16 = FileStoreTemplate<uint16_t>;
using FileStore32 = FileStoreTemplate<uint32_t>;
using FileStore64 = FileStoreTemplate<uint64_t>;

}
