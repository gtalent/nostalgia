/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ptr.hpp"

namespace ox {
namespace fs {

template<typename size_t>
class LinkedList {

	private:
		struct Header {
			ox::LittleEndian<size_t> size = sizeof(Header);
			ox::LittleEndian<size_t> firstItem = 0;
		};

		struct Item {
			ox::LittleEndian<size_t> size = sizeof(Item);
			ox::LittleEndian<size_t> prev = 0;
			ox::LittleEndian<size_t> next = 0;

			Item(size_t size) {
				this->size = size;
			}
		};

		struct ItemPtr: public ox::fs::Ptr<Item, size_t> {
			inline ItemPtr(void *dataStart, void *dataEnd, size_t itemOffset, size_t size):
			Ptr<Item, size_t>(dataStart, dataEnd, itemOffset, size) {}

			inline ItemPtr(void *dataStart, void *dataEnd, size_t itemOffset) {
				// make sure this can be read as an Item, and then use Item::size for the size
				uint8_t *itemStart = static_cast<uint8_t*>(dataStart) + itemOffset;
				if (static_cast<uint8_t*>(dataEnd) - itemStart >= static_cast<size_t>(sizeof(Item))) {
					auto item = (Item*) (static_cast<uint8_t*>(dataStart) + itemOffset);
					ItemPtr(dataStart, dataEnd, itemOffset, item->size);
				} else {
					ItemPtr(dataStart, dataEnd, 0, 0);
				}
			}
		};

		Header m_header;

	public:	
		ItemPtr firstItem();

		ItemPtr prev(Item *item);

		ItemPtr next(Item *item);

		ItemPtr ptr(size_t offset);

		ItemPtr ptr(uint8_t *item);

	//private:
		ItemPtr malloc(size_t size);

		void compact(void (*cb)(ItemPtr itemMoved));

		uint8_t *data();

};

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::firstItem() {
	return ptr(m_header.firstItem);
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
	return ItemPtr(this, this + 1, offset);
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::ptr(uint8_t *item) {
	return ItemPtr(this, this + 1, reinterpret_cast<size_t>(static_cast<uint8_t*>(item - static_cast<uint8_t*>(this))));
}

template<typename size_t>
typename LinkedList<size_t>::ItemPtr LinkedList<size_t>::malloc(size_t size) {
	auto out = ItemPtr(this, this + 1, 0, size);
	if (out.valid()) {
		new (out) Item(size);
	}
	return out;
}

template<typename size_t>
void LinkedList<size_t>::compact(void (*cb)(ItemPtr)) {
	auto src = ptr(firstItem());
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
}
