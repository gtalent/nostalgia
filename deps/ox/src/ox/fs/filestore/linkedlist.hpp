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

template<typename size_t>
class __attribute__((packed)) LinkedList {

	public:
		struct __attribute__((packed)) Item {
			friend LinkedList;

			public:
				ox::LittleEndian<size_t> m_size = sizeof(Item);

			protected:
				ox::LittleEndian<size_t> prev = 0;
				ox::LittleEndian<size_t> next = 0;

			public:
				explicit Item(size_t size) {
					this->m_size = size;
				}

				size_t size() const {
					return m_size;
				}

				ox::fs::Ptr<uint8_t, size_t> data() {
					return Ptr<uint8_t, size_t>(this, m_size, sizeof(*this), m_size - sizeof(*this));
				}
		};

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
		LinkedList() = default;

		explicit LinkedList(size_t size);

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
		 * @return the bytes still available in this LinkedList
		 */
		size_t available();

	private:
		void compact(void (*cb)(ItemPtr itemMoved));

		uint8_t *data();

};

template<typename size_t>
LinkedList<size_t>::LinkedList(size_t size) {
	m_header.size = size;
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::firstItem() {
	return ptr(m_header.firstItem);
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::lastItem() {
	auto first = ptr(m_header.firstItem);
	if (first.valid()) {
		return prev(first);
	}
	return ItemPtr();
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::prev(Item *item) {
	return ptr(item->prev);
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::next(Item *item) {
	return ptr(item->next);
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::ptr(size_t offset) {
	return ItemPtr(this, m_header.size, offset);
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::ptr(void *item) {
	return ItemPtr(this, m_header.size, reinterpret_cast<size_t>(static_cast<uint8_t*>(item) - static_cast<uint8_t*>(this)));
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::malloc(size_t size) {
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

template<typename size_t>
void LinkedList<size_t>::free(ItemPtr item) {
	auto prev = this->prev(item);
	auto next = this->next(item);
	prev->next = next;
	next->prev = prev;
	m_header.bytesUsed -= item.size();
}

template<typename size_t>
Error LinkedList<size_t>::setSize(size_t size) {
	auto last = lastItem();
	if ((last.valid() and last.end() >= size) or size < sizeof(m_header)) {
		return 1;
	} else {
		m_header.size = size;
		return 0;
	}
}

template<typename size_t>
bool LinkedList<size_t>::valid(size_t maxSize) {
	return m_header.size <= maxSize;
}

template<typename size_t>
size_t LinkedList<size_t>::available() {
	return m_header.size - m_header.bytesUsed;
}

template<typename size_t>
void LinkedList<size_t>::compact(void (*cb)(ItemPtr)) {
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
		if (prev) {
			prev->next = dest;
		}
		auto next = ptr(dest->next);
		if (next) {
			next->prev = dest;
		}
		// update iterators
		src = ptr(dest->next);
		dest += ptr(dest)->size;
	}
}

template<typename size_t>
uint8_t *LinkedList<size_t>::data() {
	return reinterpret_cast<uint8_t*>(this + 1);
}

}
