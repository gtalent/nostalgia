/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/ptrarith/nodebuffer.hpp>

namespace ox {

using InodeId_t = uint64_t;
using FsSize_t = std::size_t;

struct StatInfo {
	InodeId_t inode = 0;
	InodeId_t links = 0;
	FsSize_t size = 0;
	uint8_t fileType = 0;
};

template<typename size_t>
struct OX_PACKED FileStoreItem: public ptrarith::Item<size_t> {
	LittleEndian<size_t> id = 0;
	LittleEndian<uint8_t> fileType = 0;
	LittleEndian<size_t> links = 0;
	LittleEndian<size_t> left = 0;
	LittleEndian<size_t> right = 0;

	FileStoreItem() = default;

	explicit FileStoreItem(size_t size) {
		this->setSize(size);
	}

	/**
	 * @return the size of the data + the size of the Item type
	 */
	[[nodiscard]]
	size_t fullSize() const {
		return sizeof(*this) + this->size();
	}

	ptrarith::Ptr<uint8_t, std::size_t> data() {
		return ptrarith::Ptr<uint8_t, std::size_t>(this, this->fullSize(), sizeof(*this), this->size());
	}

};


template<typename size_t>
class FileStoreTemplate {

	public:
		using InodeId_t = size_t;

	private:
		using Item = FileStoreItem<size_t>;
		using ItemPtr = typename ptrarith::NodeBuffer<size_t, FileStoreItem<size_t>>::ItemPtr;
		using Buffer = ptrarith::NodeBuffer<size_t, FileStoreItem<size_t>>;

		static constexpr InodeId_t ReservedInodeEnd = 100;

		struct OX_PACKED FileStoreData {
			LittleEndian<size_t> rootNode = 0;
			Random random;
		};

		size_t m_buffSize = 0;
		mutable Buffer *m_buffer = nullptr;

	public:
		FileStoreTemplate() = default;

		FileStoreTemplate(void *buff, size_t buffSize);

		static Error format(void *buffer, size_t bufferSize);

		Error setSize(InodeId_t buffSize);

		Error incLinks(InodeId_t id);

		Error decLinks(InodeId_t id);

		Error write(InodeId_t id, const void *data, FsSize_t dataSize, uint8_t fileType = 0);

		Error remove(InodeId_t id);

		Error read(InodeId_t id, void *out, FsSize_t outSize, FsSize_t *size = nullptr) const;

		Error read(InodeId_t id, FsSize_t readStart, FsSize_t readSize, void *data, FsSize_t *size = nullptr) const;

		ptrarith::Ptr<uint8_t, std::size_t> read(InodeId_t id) const;

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
		Error read(InodeId_t id, FsSize_t readStart,
		           FsSize_t readSize, T *data,
		           FsSize_t *size) const;

		Result<StatInfo> stat(InodeId_t id);

		Error resize();

		Error resize(std::size_t size, void *newBuff = nullptr);

		[[nodiscard]]
		InodeId_t spaceNeeded(FsSize_t size);

		[[nodiscard]]
		InodeId_t size() const;

		[[nodiscard]]
		InodeId_t available();

		[[nodiscard]]
		char *buff();

		Error walk(Error(*cb)(uint8_t, uint64_t, uint64_t));

		Result<InodeId_t> generateInodeId();

		bool valid() const;

		Error compact();

	private:
		FileStoreData *fileStoreData() const;

		/**
		 * Places the given Item at the given ID. If it already exists, the
		 * existing value will be overwritten.
		 */
		Error placeItem(ItemPtr item);

		/**
		 * Places the given Item at the given ID. If it already exists, the
		 * existing value will be overwritten.
		 */
		Error placeItem(ItemPtr root, ItemPtr item, int depth = 0);

		/**
		 * Removes the given Item at the given ID. If it already exists, the
		 * existing value will be overwritten.
		 */
		Error unplaceItem(ItemPtr item);

		/**
		 * Removes the given Item at the given ID. If it already exists, the
		 * existing value will be overwritten.
		 */
		Error unplaceItem(ItemPtr root, ItemPtr item, int depth = 0);

		Error remove(ItemPtr item);

		/**
		 * Finds the parent an inode by its ID.
		 */
		ItemPtr findParent(ItemPtr item, size_t id, size_t oldAddr) const;

		/**
		 * Finds an inode by its ID.
		 */
		ItemPtr find(ItemPtr item, InodeId_t id, int depth = 0) const;

		/**
		 * Finds an inode by its ID.
		 */
		ItemPtr find(InodeId_t id) const;

		/**
		 * Gets the root inode.
		 */
		ItemPtr rootInode();

		bool canWrite(ItemPtr existing, size_t size);

};

template<typename size_t>
FileStoreTemplate<size_t>::FileStoreTemplate(void *buff, size_t buffSize) {
	m_buffSize = buffSize;
	m_buffer = reinterpret_cast<ptrarith::NodeBuffer<size_t, FileStoreItem<size_t>>*>(buff);
	if (!m_buffer->valid(buffSize)) {
		m_buffSize = 0;
		m_buffer = nullptr;
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::format(void *buffer, size_t bufferSize) {
	auto nb = new (buffer) Buffer(bufferSize);
	auto fsData = nb->malloc(sizeof(FileStoreData)).value;
	if (!fsData.valid()) {
		oxTrace("ox::fs::FileStoreTemplate::format::fail", "Could not read data section of FileStoreData");
		return OxError(1, "Could not read data section of FileStoreData");
	}
	auto data = nb->template dataOf<FileStoreData>(fsData);
	if (!data.valid()) {
		oxTrace("ox::fs::FileStoreTemplate::format::fail", "Could not read data section of FileStoreData");
		return OxError(1, "Could not read data section of FileStoreData");
	}
	new (data) FileStoreData;
	return OxError(0);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::setSize(InodeId_t size) {
	if (m_buffSize >= size) {
		return m_buffer->setSize(size);
	}
	return OxError(1);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::incLinks(InodeId_t id) {
	oxRequireM(item, find(id).validate());
	item->links++;
	return OxError(0);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::decLinks(InodeId_t id) {
	oxRequireM(item, find(id).validate());
	item->links--;
	if (item->links == 0) {
		oxReturnError(remove(item));
	}
	return OxError(0);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::write(InodeId_t id, const void *data, FsSize_t dataSize, uint8_t fileType) {
	oxTracef("ox::fs::FileStoreTemplate::write", "Attempting to write to inode {}", id);
	auto existing = find(id);
	if (!canWrite(existing, dataSize)) {
		oxReturnError(compact());
		existing = find(id);
	}

	if (canWrite(existing, dataSize)) {
		// delete the old node if it exists
		if (existing.valid()) {
			oxTracef("ox::fs::FileStoreTemplate::write", "Freeing old version of inode found at offset: {}", existing.offset());
			auto err = m_buffer->free(existing);
			if (err) {
				oxTrace("ox::fs::FileStoreTemplate::write::fail", "Free of old version of inode failed");
				return err;
			}
			existing = nullptr;
		}
		// write the given data
		auto dest = m_buffer->malloc(dataSize).value;
		// if first malloc failed, compact and try again
		if (!dest.valid()) {
			oxTrace("ox::fs::FileStoreTemplate::write", "Allocation failed, compacting");
			oxReturnError(compact());
			dest = m_buffer->malloc(dataSize).value;
		}
		if (dest.valid()) {
			oxTrace("ox::fs::FileStoreTemplate::write", "Memory allocated");
			dest->id = id;
			dest->fileType = fileType;
			auto destData = m_buffer->template dataOf<uint8_t>(dest);
			if (destData.valid()) {
				oxAssert(destData.size() == dataSize, "Allocation size does not match data.");
				// write data if any was provided
				if (data != nullptr) {
					ox_memcpy(destData, data, dest->size());
					oxTrace("ox::fs::FileStoreTemplate::write", "Data written");
				}
				auto fsData = fileStoreData();
				if (fsData) {
					oxTracef("ox::fs::FileStoreTemplate::write", "Searching for root node at {}", fsData->rootNode.get());
					auto root = m_buffer->ptr(fsData->rootNode);
					if (root.valid()) {
						oxTracef("ox::fs::FileStoreTemplate::write",
						         "Placing {} on {} at {}", dest->id.get(), root->id.get(), destData.offset());
						return placeItem(dest);
					} else {
						oxTracef("ox::fs::FileStoreTemplate::write",
						        "Initializing root inode: {} (offset: {}, data size: {})",
						        dest->id.get(), dest.offset(), destData.size());
						fsData->rootNode = dest.offset();
						oxTracef("ox::fs::FileStoreTemplate::write", "Root inode: {}", dest->id.get());
						return OxError(0);
					}
				} else {
					oxTrace("ox::fs::FileStoreTemplate::write::fail", "Could not place item due to absence of FileStore header.");
				}
			}
		}
		oxReturnError(m_buffer->free(dest));
	}
	return OxError(1);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::remove(InodeId_t id) {
	return remove(find(id));
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, void *out, FsSize_t outSize, FsSize_t *size) const {
	oxTracef("ox::fs::FileStoreTemplate::read", "Attempting to read from inode {}", id);
	auto src = find(id);
	// error check
	if (!src.valid()) {
		oxTracef("ox::fs::FileStoreTemplate::read::fail", "Could not find requested item: {}", id);
		return OxError(1);
	}

	auto srcData = m_buffer->template dataOf<uint8_t>(src);
	oxTracef("ox::fs::FileStoreTemplate::read::found", "{} found at {} with data section at {}",
	        id, src.offset(), srcData.offset());
	oxTracef("ox::fs::FileStoreTemplate::read::outSize", "{} {} {}", srcData.offset(), srcData.size(), outSize);

	// error check
	if (!(srcData.valid() && srcData.size() <= outSize)) {
		oxTracef("ox::fs::FileStoreTemplate::read::fail", "Could not read data section of item: {}", id);
		oxTracef("ox::fs::FileStoreTemplate::read::fail",
		         "Item data section size: {}, Expected size: {}", srcData.size(), outSize);
		return OxError(1);
	}

	ox_memcpy(out, srcData, srcData.size());
	if (size) {
		*size = src.size();
	}

	return OxError(0);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, FsSize_t readStart, FsSize_t readSize, void *data, FsSize_t *size) const {
	oxRequireM(src, find(id).validate());
	oxRequireM(srcData, src->data().validate());
	oxRequire(sub, srcData.template subPtr<uint8_t>(readStart, readSize).validate());
	memcpy(data, sub, sub.size());
	if (size) {
		*size = sub.size();
	}
	return OxError(0);
}

template<typename size_t>
template<typename T>
Error FileStoreTemplate<size_t>::read(InodeId_t id, FsSize_t readStart,
                                      FsSize_t readSize, T *data, FsSize_t *size) const {
	oxRequireM(src, find(id).validate());
	oxRequireM(srcData, src->data().validate());
	auto sub = srcData.template subPtr<uint8_t>(readStart, readSize);
	if (sub.valid() && sub.size() % sizeof(T)) {
		for (FsSize_t i = 0; i < sub.size() / sizeof(T); i++) {
			// do byte-by-byte copy to ensure alignment is right when
			// copying to final destination
			T tmp;
			for (size_t ii = 0; ii < sizeof(T); ii++) {
				reinterpret_cast<uint8_t*>(&tmp)[ii] = *(sub.get() + ii);
			}
			*(data + i) = tmp;
		}
		if (size) {
			*size = sub.size();
		}
		return OxError(0);
	}
	return OxError(1);
}

template<typename size_t>
ptrarith::Ptr<uint8_t, std::size_t> FileStoreTemplate<size_t>::read(InodeId_t id) const {
	auto item = find(id);
	if (item.valid()) {
		return item->data();
	} else {
		return nullptr;
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::resize() {
	oxReturnError(compact());
	const auto newSize = size() - available();
	oxTracef("ox::fs::FileStoreTemplate::resize", "resize to: {}", newSize);
	oxReturnError(m_buffer->setSize(newSize));
	oxTracef("ox::fs::FileStoreTemplate::resize", "resized to: {}", m_buffer->size());
	return OxError(0);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::resize(std::size_t size, void *newBuff) {
	if (m_buffer->size() > size) {
		return OxError(1);
	}
	m_buffSize = size;
	if (newBuff) {
		m_buffer = reinterpret_cast<Buffer*>(newBuff);
		oxReturnError(m_buffer->setSize(size));
	}
	return OxError(0);
}

template<typename size_t>
Result<StatInfo> FileStoreTemplate<size_t>::stat(InodeId_t id) {
	oxRequire(inode, find(id).validate());
	return StatInfo {
		id,
		inode->links,
		inode->size(),
		inode->fileType,
	};
}

template<typename size_t>
typename FileStoreTemplate<size_t>::InodeId_t FileStoreTemplate<size_t>::spaceNeeded(FsSize_t size) {
	return m_buffer->spaceNeeded(size);
}

template<typename size_t>
typename FileStoreTemplate<size_t>::InodeId_t FileStoreTemplate<size_t>::size() const {
	return m_buffer->size();
}

template<typename size_t>
typename FileStoreTemplate<size_t>::InodeId_t FileStoreTemplate<size_t>::available() {
	return m_buffer->available();
}

template<typename size_t>
char *FileStoreTemplate<size_t>::buff() {
	return reinterpret_cast<char*>(m_buffer);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::walk(Error(*cb)(uint8_t, uint64_t, uint64_t)) {
	for (auto i = m_buffer->iterator(); i.valid(); i.next()) {
		oxReturnError(cb(i->fileType, i.ptr().offset(), i.ptr().end()));
	}
	return OxError(0);
}

template<typename size_t>
Result<typename FileStoreTemplate<size_t>::InodeId_t> FileStoreTemplate<size_t>::generateInodeId() {
	auto fsData = fileStoreData();
	if (!fsData) {
		return OxError(1);
	}
	for (auto i = 0; i < 100; i++) {
		auto inode = fsData->random.gen() % MaxValue<InodeId_t>;
		if (inode > ReservedInodeEnd && !find(inode).valid()) {
			return inode;
		}
	}
	return OxError(2);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::compact() {
	auto isFirstItem = true;
	return m_buffer->compact([this, &isFirstItem](uint64_t oldAddr, ItemPtr item) -> Error {
		if (isFirstItem) {
			isFirstItem = false;
			return OxError(0);
		}
		if (!item.valid()) {
			return OxError(1);
		}
		oxTracef("ox::FileStoreTemplate::compact::moveItem", "Moving Item: {} from {} to {}", item->id.get(), oldAddr, item.offset());
		// update rootInode if this is it
		auto fsData = fileStoreData();
		if (fsData && oldAddr == fsData->rootNode) {
			fsData->rootNode = item.offset();
		}
		auto parent = findParent(rootInode(), item->id, oldAddr);
		oxAssert(parent.valid() || rootInode() == item.offset(),
		         "Parent inode not found for item that should have parent.");
		if (parent.valid()) {
			if (parent->left == oldAddr) {
				parent->left = item;
			} else if (parent->right == oldAddr) {
				parent->right = item;
			}
		}
		return OxError(0);
	});
}

template<typename size_t>
typename FileStoreTemplate<size_t>::FileStoreData *FileStoreTemplate<size_t>::fileStoreData() const {
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
Error FileStoreTemplate<size_t>::placeItem(ItemPtr item) {
	auto fsData = fileStoreData();
	if (!fsData) {
		return OxError(1);
	}
	oxRequireM(root, m_buffer->ptr(fsData->rootNode).validate());
	if (root->id == item->id) {
		fsData->rootNode = item;
		item->left = root->left;
		item->right = root->right;
		oxTracef("ox::fs::FileStoreTemplate::placeItem", "Overwrote Root Item: {}", item->id.get());
		return OxError(0);
	} else {
		return placeItem(root, item);
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::placeItem(ItemPtr root, ItemPtr item, int depth) {
	if (depth > 5000) {
		oxTrace("ox::fs::FileStoreTemplate::placeItem::fail", "Excessive recursion depth, stopping before stack overflow.");
		return OxError(2);
	}
	if (item->id > root->id) {
		auto right = m_buffer->ptr(root->right);
		if (!right.valid() || right->id == item->id) {
			root->right = item.offset();
			if (right.valid()) {
				item->left = right->left;
				item->right = right->right;
			}
			oxTracef("ox::fs::FileStoreTemplate::placeItem", "Placed Item: {}", item->id.get());
			return OxError(0);
		} else {
			return placeItem(right, item, depth + 1);
		}
	} else if (item->id < root->id) {
		auto left = m_buffer->ptr(root->left);
		if (!left.valid() || left->id == item->id) {
			root->left = item.offset();
			if (left.valid()) {
				item->left = left->left;
				item->right = left->right;
			}
			oxTracef("ox::fs::FileStoreTemplate::placeItem", "Placed Item: {}", item->id.get());
			return OxError(0);
		} else {
			return placeItem(left, item, depth + 1);
		}
	} else {
		oxTrace("ox::fs::FileStoreTemplate::placeItem::fail", "Cannot insert an item on itself.");
		return OxError(1, "Cannot insert an item on itself.");
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::unplaceItem(ItemPtr item) {
	auto fsData = fileStoreData();
	if (!fsData) {
		return OxError(1);
	}
	oxRequireM(root, m_buffer->ptr(fsData->rootNode).validate());
	if (root->id == item->id) {
		item->left = root->left;
		item->right = root->right;
		auto left = m_buffer->ptr(item->left);
		auto right = m_buffer->ptr(item->right);
		if (right.valid()) {
			auto oldRoot = fsData->rootNode;
			fsData->rootNode = item->right;
			if (left.valid()) {
				auto err = placeItem(left);
				// undo if unable to place the left side of the tree
				if (err) {
					fsData->rootNode = oldRoot;
					return err;
				}
			}
		} else if (left.valid()) {
			fsData->rootNode = item->left;
		} else {
			fsData->rootNode = 0;
		}
		return OxError(0);
	} else {
		return unplaceItem(root, item);
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::unplaceItem(ItemPtr root, ItemPtr item, int depth) {
	if (depth >= 5000) {
		oxTrace("ox::fs::FileStoreTemplate::unplaceItem::fail", "Excessive recursion depth, stopping before stack overflow.");
		return OxError(1, "Excessive recursion depth, stopping before stack overflow.");
	}
	if (item->id > root->id) {
		auto right = m_buffer->ptr(root->right);
		if (right->id == item->id) {
			root->right = 0;
			oxTracef("ox::fs::FileStoreTemplate::unplaceItem", "Unplaced Item: {}", item->id.get());
		} else {
			return unplaceItem(right, item, depth + 1);
		}
	} else if (item->id < root->id) {
		auto left = m_buffer->ptr(root->left);
		if (left->id == item->id) {
			root->left = 0;
			oxTracef("ox::fs::FileStoreTemplate::unplaceItem", "Unplaced Item: {}", item->id.get());
		} else {
			return unplaceItem(left, item, depth + 1);
		}
	} else {
		return OxError(1);
	}
	if (item->right) {
		oxReturnError(placeItem(m_buffer->ptr(item->right)));
	}
	if (item->left) {
		oxReturnError(placeItem(m_buffer->ptr(item->left)));
	}
	return OxError(0);
}

template<typename size_t>
Error FileStoreTemplate<size_t>::remove(ItemPtr item) {
	if (item.valid()) {
		oxReturnError(unplaceItem(item));
		oxReturnError(m_buffer->free(item));
		return OxError(0);
	}
	return OxError(1);
}

template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::findParent(ItemPtr item, size_t id, size_t oldAddr) const {
	// This is a little bit confusing. findParent uses the inode ID to find
	// where the target ID should be, but the actual address of that item is
	// currently invalid, so we check it against what is known to be the old
	// address of the item to confirm that we have the right item.
	if (item.valid()) {
		if (id > item->id) {
			if (item->right == oldAddr) {
				return item;
			} else {
				auto right = m_buffer->ptr(item->right);
				return findParent(right, id, oldAddr);
			}
		} else if (id < item->id) {
			if (item->left == oldAddr) {
				return item;
			} else {
				auto left = m_buffer->ptr(item->left);
				return findParent(left, id, oldAddr);
			}
		}
	}
	return nullptr;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::find(ItemPtr item, InodeId_t id, int depth) const {
	if (depth > 5000) {
		oxTracef("ox::fs::FileStoreTemplate::find::fail", "Excessive recursion depth, stopping before stack overflow. Search for: {}", id);
		return nullptr;
	}
	if (!item.valid()) {
		oxTrace("ox::fs::FileStoreTemplate::find::fail", "item invalid");
		return nullptr;
	}
	// do search
	if (id > item->id) {
		oxTracef("ox::fs::FileStoreTemplate::find", "Not a match, searching on {}", item->right.get());
		return find(m_buffer->ptr(item->right), id, depth + 1);
	} else if (id < item->id) {
		oxTracef("ox::fs::FileStoreTemplate::find", "Not a match, searching on {}", item->left.get());
		return find(m_buffer->ptr(item->left), id, depth + 1);
	} else if (id == item->id) {
		oxTracef("ox::fs::FileStoreTemplate::find", "Found {} at {}", id, item.offset());
		return item;
	}
	return nullptr;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::ItemPtr FileStoreTemplate<size_t>::find(InodeId_t id) const {
	oxTracef("ox::fs::FileStoreTemplate::find", "Searching for inode: {}", id);
	auto fsData = fileStoreData();
	if (fsData) {
		auto root = m_buffer->ptr(fsData->rootNode);
		if (root.valid()) {
			auto item = find(root, id);
			return item;
		} else {
			oxTrace("ox::fs::FileStoreTemplate::find::fail", "No root node");
		}
	} else {
		oxTrace("ox::fs::FileStoreTemplate::find::fail", "No FileStore Data");
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

template<typename size_t>
bool FileStoreTemplate<size_t>::valid() const {
	return m_buffer;
}

extern template class FileStoreTemplate<uint16_t>;
extern template class FileStoreTemplate<uint32_t>;

using FileStore16 = FileStoreTemplate<uint16_t>;
using FileStore32 = FileStoreTemplate<uint32_t>;

}
