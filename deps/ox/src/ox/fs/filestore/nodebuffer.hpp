/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/trace/trace.hpp>

#include "ptr.hpp"

namespace ox::fs {

template<typename size_t, typename Item>
class __attribute__((packed)) NodeBuffer {

	public:
		struct __attribute__((packed)) Header {
			ox::LittleEndian<size_t> size = sizeof(Header);
			ox::LittleEndian<size_t> bytesUsed = sizeof(Header);
			ox::LittleEndian<size_t> firstItem = 0;
		};

		struct ItemPtr: public ox::fs::Ptr<Item, size_t, sizeof(Header)> {
			inline ItemPtr() = default;

			inline ItemPtr(std::nullptr_t) {
			}

			inline ItemPtr(void *dataStart, size_t dataSize, size_t itemOffset, size_t size):
			Ptr<Item, size_t, sizeof(Header)>(dataStart, dataSize, itemOffset, size) {
			}

			inline ItemPtr(void *dataStart, size_t dataSize, size_t itemOffset) {
				// make sure this can be read as an Item, and then use Item::size for the size
				auto itemSpace = dataSize - itemOffset;
				auto item = reinterpret_cast<Item*>(static_cast<uint8_t*>(dataStart) + itemOffset);
				if (itemOffset >= sizeof(Header) and
				    itemOffset < dataSize - sizeof(Item) and
				    itemSpace >= static_cast<size_t>(sizeof(Item)) and
				    itemSpace >= item->fullSize()) {
					this->init(dataStart, dataSize, itemOffset, sizeof(item) + item->fullSize());
				} else {
					this->init(dataStart, dataSize, 0, 0);
				}
			}
		};

		Header m_header;

	public:
		NodeBuffer() = default;

		explicit NodeBuffer(size_t size);

		ItemPtr firstItem();

		ItemPtr lastItem();

		ItemPtr prev(Item *item);

		ItemPtr next(Item *item);

		ItemPtr ptr(size_t offset);

		ItemPtr ptr(void *item);

		ItemPtr malloc(size_t size);

		void free(ItemPtr item);

		bool valid(size_t maxSize);

		Error setSize(size_t size);

		size_t size();

		/**
		 * @return the bytes still available in this NodeBuffer
		 */
		size_t available();

		/**
		 * @return the actual number a bytes need to store the given number of
		 * bytes
		 */
		size_t spaceNeeded(size_t size);

	private:
		void compact(void (*cb)(ItemPtr itemMoved));

		uint8_t *data();

};

template<typename size_t, typename Item>
NodeBuffer<size_t, Item>::NodeBuffer(size_t size) {
	m_header.size = size;
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::firstItem() {
	return ptr(m_header.firstItem);
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::lastItem() {
	auto first = ptr(m_header.firstItem);
	if (first.valid()) {
		return prev(first);
	}
	return nullptr;
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::prev(Item *item) {
	return ptr(item->prev);
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::next(Item *item) {
	return ptr(item->next);
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::ptr(size_t offset) {
	return ItemPtr(this, m_header.size, offset);
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::malloc(size_t size) {
	auto fullSize = size + sizeof(Item);
	if (m_header.size - m_header.bytesUsed >= fullSize) {
		auto last = lastItem();
		size_t addr = 0;
		if (last.valid()) {
			addr = last.offset() + last.size();
		} else {
			// there is no first item, so this may be the first item
			if (!m_header.firstItem) {
				oxTrace("ox::fs::NodeBuffer::malloc") << "No first item.";
				m_header.firstItem = sizeof(m_header);
				addr = m_header.firstItem;
			}
		}
		auto out = ItemPtr(this, m_header.size, addr, fullSize);
		if (out.valid()) {
			new (out) Item(size);

			auto first = firstItem();
			out->next = first;
			if (first.valid()) {
				first->prev = out;
			} else {
				oxTrace("ox::fs::NodeBuffer::malloc::fail") << "NodeBuffer malloc failed due to invalid first element pointer.";
				return nullptr;
			}

			auto last = lastItem();
			out->prev = last;
			if (last.valid()) {
				last->next = out;
			} else {
				oxTrace("ox::fs::NodeBuffer::malloc::fail") << "NodeBuffer malloc failed due to invalid last element pointer.";
				return nullptr;
			}
			m_header.bytesUsed += out.size();
		}
		return out;
	}
	return nullptr;
}

template<typename size_t, typename Item>
void NodeBuffer<size_t, Item>::free(ItemPtr item) {
	auto prev = this->prev(item);
	auto next = this->next(item);
	if (prev.valid()) {
		prev->next = next;
	} else {
		oxTrace("ox::fs::NodeBuffer::free::fail") << "NodeBuffer free failed due to invalid prev element pointer.";
	}
	if (next.valid()) {
		next->prev = prev;
	} else {
		oxTrace("ox::fs::NodeBuffer::free::fail") << "NodeBuffer free failed due to invalid next element pointer.";
	}
	m_header.bytesUsed -= item.size();
}

template<typename size_t, typename Item>
Error NodeBuffer<size_t, Item>::setSize(size_t size) {
	auto last = lastItem();
	if ((last.valid() and last.end() >= size) or size < sizeof(m_header)) {
		return 1;
	} else {
		m_header.size = size;
		return 0;
	}
}

template<typename size_t, typename Item>
size_t NodeBuffer<size_t, Item>::size() {
	return m_header.size;
}

template<typename size_t, typename Item>
bool NodeBuffer<size_t, Item>::valid(size_t maxSize) {
	return m_header.size <= maxSize;
}

template<typename size_t, typename Item>
size_t NodeBuffer<size_t, Item>::available() {
	return m_header.size - m_header.bytesUsed;
}

template<typename size_t, typename Item>
size_t NodeBuffer<size_t, Item>::spaceNeeded(size_t size) {
	return sizeof(Item) + size;
}

template<typename size_t, typename Item>
void NodeBuffer<size_t, Item>::compact(void (*cb)(ItemPtr)) {
	auto src = firstItem();
	auto dest = ptr(sizeof(*this));
	while (src.valid() && dest.valid()) {
		// move node
		ox_memcpy(dest, src, src.size());
		if (cb) {
			cb(dest);
		}
		// update surrounding nodes
		auto prev = ptr(dest->next);
		if (prev.valid()) {
			prev->next = dest;
		}
		auto next = ptr(dest->next);
		if (next.valid()) {
			next->prev = dest;
		}
		// update iterators
		src = ptr(dest->next);
		dest = ptr(dest.offset() + dest.size());
	}
}

template<typename size_t, typename Item>
uint8_t *NodeBuffer<size_t, Item>::data() {
	return reinterpret_cast<uint8_t*>(ptr(sizeof(*this)).get());
}


template<typename size_t>
struct __attribute__((packed)) Item {
	public:
		ox::LittleEndian<size_t> prev = 0;
		ox::LittleEndian<size_t> next = 0;

	private:
		ox::LittleEndian<size_t> m_size = sizeof(Item);

	public:
		explicit Item(size_t size) {
			this->m_size = size;
		}

		size_t size() const {
			return m_size;
		}
};

}
