/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/ptrarith/nodebuffer.hpp>

#include "filestore.hpp"

namespace ox::fs {

template<typename size_t>
struct __attribute__((packed)) FileStoreItem: public ptrarith::Item<size_t> {
	ox::LittleEndian<size_t> id = 0;
	ox::LittleEndian<uint8_t> fileType = 0;
	ox::LittleEndian<size_t> links = 0;
	ox::LittleEndian<size_t> left = 0;
	ox::LittleEndian<size_t> right = 0;

	explicit FileStoreItem(size_t size): ptrarith::Item<size_t>(size) {
	}

	/**
	 * @return the size of the data + the size of the Item type
	 */
	size_t fullSize() const {
		return sizeof(*this) + this->size();
	}

	ox::ptrarith::Ptr<uint8_t, size_t> data() {
		return ptrarith::Ptr<uint8_t, size_t>(this, this->size(), sizeof(*this), this->size() - sizeof(*this));
	}
};


template<typename size_t>
class FileStoreTemplate: public FileStore {

	private:
		using ItemPtr = typename ox::ptrarith::NodeBuffer<size_t, FileStoreItem<size_t>>::ItemPtr;
		using Buffer = ox::ptrarith::NodeBuffer<size_t, FileStoreItem<size_t>>;

		struct __attribute__((packed)) FileStoreData {
			ox::LittleEndian<size_t> rootNode = 0;
		};

		size_t m_buffSize = 0;
		Buffer *m_buffer = nullptr;

	public:
		FileStoreTemplate(void *buff, size_t buffSize);

		Error format();

		Error setSize(InodeId_t buffSize);

		Error incLinks(InodeId_t id);

		Error decLinks(InodeId_t id);

		Error write(InodeId_t id, void *data, FsSize_t dataLen, uint8_t fileType = 0);

		Error read(InodeId_t id, void *data, FsSize_t dataSize, FsSize_t *size);

		Error read(InodeId_t id, FsSize_t readStart, FsSize_t readSize, void *data, FsSize_t *size);

		/**
		 * Reads the "file" at the given id. You are responsible for freeing
		 * the data when done with it.
		 * @param id id of the "file"
		 * @param readStart where in the data to start reading
		 * @param readSize how much data to read
		 * @param data pointer to the pointer where the data is stored
		 * @param size pointer to a value that will be assigned the size of data
		 * @return 0 if read is a success
		 */
		template<typename T>
		int read(InodeId_t id, FsSize_t readStart,
		         FsSize_t readSize, T *data,
		         FsSize_t *size);

		StatInfo stat(InodeId_t id);

		InodeId_t spaceNeeded(FsSize_t size);

		InodeId_t size();

		InodeId_t available();

	private:
		FileStoreData *fileStoreData();

		/**
		 * Places the given Item at the given ID. If it already exists, the
		 * existing value will be overwritten.
		 */
		Error placeItem(ItemPtr root, ItemPtr item, int depth = 0);

		/**
		 * Finds the parent an inode by its ID.
		 */
		ItemPtr findParent(ItemPtr ptr, size_t id);

		/**
		 * Finds an inode by its ID.
		 */
		ItemPtr find(ItemPtr item, InodeId_t id, int depth = 0);

		/**
		 * Finds an inode by its ID.
		 */
		ItemPtr find(InodeId_t id);

		/**
		 * Gets the root inode.
		 */
		ItemPtr rootInode();

		bool canWrite(ItemPtr existing, size_t size);

};

template<typename size_t>
FileStoreTemplate<size_t>::FileStoreTemplate(void *buff, size_t buffSize) {
	m_buffSize = buffSize;
	m_buffer = reinterpret_cast<ox::ptrarith::NodeBuffer<size_t, FileStoreItem<size_t>>*>(buff);
	if (!m_buffer->valid(buffSize)) {
		m_buffSize = 0;
		m_buffer = nullptr;
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::format() {
	new (m_buffer) Buffer(m_buffSize);
	auto fsData = m_buffer->malloc(sizeof(FileStoreData));
	if (fsData.valid()) {
		auto data = m_buffer->template dataOf<FileStoreData>(fsData);
		if (data.valid()) {
			new (data) FileStoreData;
			return 0;
		} else {
			oxTrace("ox::fs::FileStoreTemplate::format::fail") << "Could not read data section of FileStoreData";
		}
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
Error FileStoreTemplate<size_t>::write(InodeId_t id, void *data, FsSize_t dataSize, uint8_t fileType) {
	oxTrace("ox::fs::FileStoreTemplate::write") << "Attempting to write to inode" << id;
	auto existing = find(id);
	// TODO: try compacting if unable to write
	if (canWrite(existing, dataSize)) {
		// delete the old node if it exists
		if (existing.valid()) {
			m_buffer->free(existing);
			existing = nullptr;
		}

		// write the given data
		auto dest = m_buffer->malloc(dataSize);
		// if first malloc failed, compact and try again
		dest = m_buffer->malloc(dataSize);
		if (dest.valid()) {
			new (dest) FileStoreItem<size_t>(dataSize);
			dest->id = id;
			dest->fileType = fileType;
			auto destData = m_buffer->template dataOf<uint8_t>(dest);
			if (destData.valid()) {
				ox_memcpy(destData, data, dest->size());
				auto fsData = fileStoreData();
				if (fsData) {
					auto root = m_buffer->ptr(fsData->rootNode);
					if (root.valid()) {
						oxTrace("ox::fs::FileStoreTemplate::write") << "Placing" << dest->id << "on" << root->id << "at" << destData.offset();
						return placeItem(root, dest);
					} else {
						oxTrace("ox::fs::FileStoreTemplate::write") << "Initializing root inode with offset of" << dest.offset()
							<< "and data size of" << destData.size();
						fsData->rootNode = dest.offset();
						return 0;
					}
				} else {
					oxTrace("ox::fs::FileStoreTemplate::write::fail") << "Could not place item due to absence of FileStore header.";
				}
			}
		}
		m_buffer->free(dest);
	}
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, void *data, FsSize_t dataSize, FsSize_t *size) {
	oxTrace("ox::fs::FileStoreTemplate::read") << "Attempting to read from inode" << id;
	auto src = find(id);
	if (src.valid()) {
		auto srcData = m_buffer->template dataOf<uint8_t>(src);

		oxTrace("ox::fs::FileStoreTemplate::read::found") << id << "found at"<< src.offset()
			<< "with data section at" << srcData.offset();
		oxTrace("ox::fs::FileStoreTemplate::read::outSize") << srcData.offset() << srcData.size() << dataSize;
		if (srcData.valid() && srcData.size() <= dataSize) {
			ox_memcpy(data, srcData, srcData.size());
			if (size) {
				*size = src.size();
			}
			return 0;
		} else {
			oxTrace("ox::fs::FileStoreTemplate::read::fail") << "Could not read data section of item:" << id;
			oxTrace("ox::fs::FileStoreTemplate::read::fail") << "Item data section size:" << srcData.size();
		}
	} else {
		oxTrace("ox::fs::FileStoreTemplate::read::fail") << "Could not find requested item:" << id;
	}
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, FsSize_t readStart, FsSize_t readSize, void *data, FsSize_t *size) {
	auto src = find(id);
	if (src.valid()) {
		auto srcData = src->data();
		if (srcData.valid()) {
			auto sub = srcData.template subPtr<uint8_t>(readStart, readSize);
			if (sub.valid()) {
				ox_memcpy(data, sub, sub.size());
				if (size) {
					*size = sub.size();
				}
				return 0;
			} else {
				oxTrace("ox::fs::FileStoreTemplate::read::fail") << "Could not read requested data sub-section of item:" << id;
			}
		} else {
			oxTrace("ox::fs::FileStoreTemplate::read::fail") << "Could not read data section of item:" << id;
		}
	} else {
		oxTrace("ox::fs::FileStoreTemplate::read::fail") << "Could not find requested item:" << id;
	}
	return 1;
}

template<typename size_t>
template<typename T>
int FileStoreTemplate<size_t>::read(InodeId_t id, FsSize_t readStart,
                                    FsSize_t readSize, T *data, FsSize_t *size) {
	auto src = find(id);
	if (src.valid()) {
		auto srcData = src->data();
		if (srcData.valid()) {
			auto sub = srcData.subPtr(readStart, readSize);
			if (sub.valid() && sub.size() % sizeof(T)) {
				for (FsSize_t i = 0; i < sub.size() / sizeof(T); i++) {
					// do byte-by-byte copy to ensure alignment is right when
					// copying to final destination
					T tmp;
					for (size_t i = 0; i < sizeof(T); i++) {
						*(reinterpret_cast<uint8_t*>(&tmp)[i]) = *(reinterpret_cast<uint8_t*>(sub.get()) + i);
					}
					*(data + i) = tmp;
				}
				if (size) {
					*size = sub.size();
				}
				return 0;
			}
		}
	}
	return 1;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::StatInfo FileStoreTemplate<size_t>::stat(InodeId_t id) {
	auto inode = find(id);
	if (inode.valid()) {
		return {
			.inodeId = id,
			.links = inode->links,
			.size = inode->size(),
			.fileType = inode->fileType,
		};
	}
	return {};
}

template<typename size_t>
InodeId_t FileStoreTemplate<size_t>::spaceNeeded(FsSize_t size) {
	return m_buffer->spaceNeeded(size);
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
typename FileStoreTemplate<size_t>::FileStoreData *FileStoreTemplate<size_t>::fileStoreData() {
	auto first = m_buffer->firstItem();
	if (first.valid()) {
		auto data = first->data();
		if (data.valid()) {
			return reinterpret_cast<FileStoreData*>(data.get());
		}
	}
	return nullptr;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::placeItem(ItemPtr root, ItemPtr item, int depth) {
	if (depth < 5000) {
		if (item->id > root->id) {
			auto right = m_buffer->ptr(root->right);
			if (!right.valid() || right->id == item->id) {
				root->right = root.offset();
				return 0;
			} else {
				return placeItem(right, item, depth + 1);
			}
		} else if (item->id < root->id) {
			auto left = m_buffer->ptr(root->left);
			if (!left.valid() || left->id == item->id) {
				root->left = root.offset();
				return 0;
			} else {
				return placeItem(left, item, depth + 1);
			}
		}
	} else {
		oxTrace("ox::fs::FileStoreTemplate::placeItem::fail") << "Excessive recursion depth, stopping before stack overflow.";
	}
	return 1;
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
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::find(ItemPtr item, InodeId_t id, int depth) {
	if (depth < 5000) {
		if (item.valid()) {
			if (id > item->id) {
				return find(m_buffer->ptr(item->right), id, depth + 1);
			} else if (id < item->id) {
				return find(m_buffer->ptr(item->left), id, depth + 1);
			} else {
				return item;
			}
		} else {
			oxTrace("ox::fs::FileStoreTemplate::find::fail") << "item invalid";
		}
	} else {
		oxTrace("ox::fs::FileStoreTemplate::find::fail") << "Excessive recursion depth, stopping before stack overflow. Search for:" << id;
	}
	return nullptr;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::find(InodeId_t id) {
	auto fsData = fileStoreData();
	if (fsData) {
		auto root = m_buffer->ptr(fsData->rootNode);
		if (root.valid()) {
			auto item = find(root, id);
			return item;
		} else {
			oxTrace("ox::fs::FileStoreTemplate::find::fail") << "No root node";
		}
	} else {
		oxTrace("ox::fs::FileStoreTemplate::find::fail") << "No FileStore Data";
	}
	return nullptr;
}

/**
 * Gets the root inode.
 */
template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::rootInode() {
	auto fsData = fileStoreData();
	if (fsData) {
		return m_buffer->ptr(fsData->rootNode);
	} else {
		return nullptr;
	}
}

template<typename size_t>
bool FileStoreTemplate<size_t>::canWrite(ItemPtr existing, size_t size) {
	return existing.size() >= size || m_buffer->spaceNeeded(size) <= m_buffer->available();
}

extern template class FileStoreTemplate<uint16_t>;
extern template class FileStoreTemplate<uint32_t>;

using FileStore16 = FileStoreTemplate<uint16_t>;
using FileStore32 = FileStoreTemplate<uint32_t>;

}
