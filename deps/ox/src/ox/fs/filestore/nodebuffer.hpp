/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ptr.hpp"

namespace ox::fs {

template<typename size_t, typename Item>
class __attribute__((packed)) NodeBuffer {

	private:
		struct __attribute__((packed)) Header {
			ox::LittleEndian<size_t> size = sizeof(Header);
			ox::LittleEndian<size_t> bytesUsed = sizeof(Header);
			ox::LittleEndian<size_t> firstItem = 0;
		};

		struct ItemPtr: public ox::fs::Ptr<Item, size_t> {
			inline ItemPtr() = default;

			inline ItemPtr(void *dataStart, size_t dataSize, size_t itemOffset, size_t size):
			Ptr<Item, size_t>(dataStart, dataSize, itemOffset, size) {
			}

			inline ItemPtr(void *dataStart, size_t dataSize, size_t itemOffset) {
				// make sure this can be read as an Item, and then use Item::size for the size
				auto itemSpace = dataSize - itemOffset;
				auto item = reinterpret_cast<Item*>(static_cast<uint8_t*>(dataStart) + itemOffset);
				if (itemOffset >= sizeof(Header) and
				    itemSpace >= static_cast<size_t>(sizeof(Item)) and
				    itemSpace >= item->size()) {
					this->init(dataStart, dataSize, itemOffset, item->size());
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
	return ItemPtr();
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
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::ptr(void *item) {
	return ItemPtr(this, m_header.size, reinterpret_cast<size_t>(static_cast<uint8_t*>(item) - static_cast<uint8_t*>(this)));
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::malloc(size_t size) {
	size += sizeof(Item);
	if (m_header.size - m_header.bytesUsed >= size) {
		if (!m_header.firstItem) {
			m_header.firstItem = sizeof(m_header);
		}
		auto out = ItemPtr(this, m_header.size, m_header.firstItem, size);
		if (out.valid()) {
			new (out) Item(size);
			auto first = firstItem();
			auto last = lastItem();
			out->next = first;
			out->prev = last;
			if (first.valid()) {
				first->prev = out;
			}
			if (last.valid()) {
				last->next = out;
			}
			m_header.bytesUsed += out.size();
		}
		return out;
	}
	return ItemPtr();
}

template<typename size_t, typename Item>
void NodeBuffer<size_t, Item>::free(ItemPtr item) {
	auto prev = this->prev(item);
	auto next = this->next(item);
	if (prev.valid()) {
		prev->next = next;
	}
	if (next.valid()) {
		next->prev = prev;
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
bool NodeBuffer<size_t, Item>::valid(size_t maxSize) {
	return m_header.size <= maxSize;
}

template<typename size_t, typename Item>
size_t NodeBuffer<size_t, Item>::available() {
	return m_header.size - m_header.bytesUsed;
}

template<typename size_t, typename Item>
void NodeBuffer<size_t, Item>::compact(void (*cb)(ItemPtr)) {
	auto src = firstItem();
	auto dest = data();
	while (src.valid()) {
		// move node
		ox_memcpy(dest, src, src.size());
		if (cb) {
			cb(ptr(dest));
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
		dest += ptr(dest)->size;
	}
}

template<typename size_t, typename Item>
uint8_t *NodeBuffer<size_t, Item>::data() {
	return reinterpret_cast<uint8_t*>(this + 1);
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
