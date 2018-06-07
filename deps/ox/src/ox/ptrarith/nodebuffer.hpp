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

namespace ox::ptrarith {

template<typename size_t, typename Item>
class __attribute__((packed)) NodeBuffer {

	public:
		struct __attribute__((packed)) Header {
			ox::LittleEndian<size_t> size = sizeof(Header);
			ox::LittleEndian<size_t> bytesUsed = sizeof(Header);
			ox::LittleEndian<size_t> firstItem = 0;
		};

		using ItemPtr = Ptr<Item, size_t, sizeof(Header)>;

		class Iterator {
			private:
				NodeBuffer *m_buffer = nullptr;
				ItemPtr m_current;
				size_t m_it = 0;

			public:
				Iterator(NodeBuffer *buffer, ItemPtr current) {
					m_buffer = buffer;
					m_current = current;
					oxTrace("ox::ptrarith::Iterator::start") << current.offset();
				}

				operator const Item*() const {
					return m_current;
				}

				Item *get() {
					return m_current;
				}

				operator Item*() {
					return m_current;
				}

				const Item *operator->() const {
					return m_current;
				}

				Item *operator->() {
					return m_current;
				}

				bool valid() const noexcept {
					return m_current.valid();
				}

				bool hasNext() {
					if (m_current.valid()) {
						oxTrace("ox::ptrarith::NodeBuffer::Iterator::hasNext::current") << m_current.offset();
						auto next = m_buffer->next(m_current);
						return next.valid() && m_buffer->firstItem() != next;
					}
					return false;
				}

				void next() {
					oxTrace("ox::ptrarith::NodeBuffer::Iterator::next") << m_it++;
					if (hasNext()) {
						m_current = m_buffer->next(m_current);
					} else {
						m_current = nullptr;
					}
				}
		};

		Header m_header;

	public:
		NodeBuffer() = default;

		NodeBuffer(const NodeBuffer &other);

		explicit NodeBuffer(size_t size);

		const Iterator iterator() const;

		Iterator iterator();

		ItemPtr firstItem();

		ItemPtr lastItem();

		/**
		 * @return the data section of the given item
		 */
		template<typename T>
		Ptr<T, size_t, sizeof(Item)> dataOf(ItemPtr);

		ItemPtr prev(Item *item);

		ItemPtr next(Item *item);

		ItemPtr ptr(size_t offset);

		ItemPtr ptr(void *item);

		ItemPtr malloc(size_t size);

		Error free(ItemPtr item);

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

		void compact(void (*cb)(ItemPtr itemMoved) = nullptr);

		void truncate();

	private:
		uint8_t *data();

};

template<typename size_t, typename Item>
NodeBuffer<size_t, Item>::NodeBuffer(size_t size) {
	m_header.size = size;
}

template<typename size_t, typename Item>
NodeBuffer<size_t, Item>::NodeBuffer(const NodeBuffer &other) {
	ox_memcpy(this, &other, other.size());
}

template<typename size_t, typename Item>
const typename NodeBuffer<size_t, Item>::Iterator NodeBuffer<size_t, Item>::iterator() const {
	return Iterator(this, firstItem());
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::Iterator NodeBuffer<size_t, Item>::iterator() {
	oxTrace("ox::ptrarith::NodeBuffer::iterator::size") << m_header.size;
	return Iterator(this, firstItem());
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::firstItem() {
	oxTrace("ox::ptrarith::NodeBuffer::firstItem") << m_header.firstItem;
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
template<typename T>
Ptr<T, size_t, sizeof(Item)> NodeBuffer<size_t, Item>::dataOf(ItemPtr ip) {
	auto out = ip.template subPtr<T>(sizeof(Item));
	oxAssert(out.size() == ip.size() - sizeof(Item), "Sub Ptr has invalid size.");
	return out;
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
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::ptr(size_t itemOffset) {
	// make sure this can be read as an Item, and then use Item::size for the size
	std::size_t itemSpace = m_header.size - itemOffset;
	auto item = reinterpret_cast<Item*>(reinterpret_cast<uint8_t*>(this) + itemOffset);
	if (itemOffset >= sizeof(Header) &&
		 itemSpace >= sizeof(Item) &&
		 itemSpace >= item->fullSize()) {
		return ItemPtr(this, m_header.size, itemOffset, item->fullSize());
	} else {
		oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << itemOffset;
		return ItemPtr(this, m_header.size, 0, 0);
	}
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::malloc(size_t size) {
	oxTrace("ox::ptrarith::NodeBuffer::malloc") << "Size:" << size;
	size_t fullSize = size + sizeof(Item);
	if (m_header.size - m_header.bytesUsed >= fullSize) {
		auto last = lastItem();
		size_t addr = 0;
		if (last.valid()) {
			addr = last.offset() + last.size();
		} else {
			// there is no first item, so this may be the first item
			if (!m_header.firstItem) {
				oxTrace("ox::ptrarith::NodeBuffer::malloc") << "No first item, initializing.";
				m_header.firstItem = sizeof(m_header);
				addr = m_header.firstItem;
			}
		}
		auto out = ItemPtr(this, m_header.size, addr, fullSize);
		if (out.valid()) {
			new (out) Item;
			out->setSize(size);

			auto first = firstItem();
			out->next = first.offset();
			if (first.valid()) {
				first->prev = out.offset();
			} else {
				oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "NodeBuffer malloc failed due to invalid first element pointer.";
				return nullptr;
			}

			auto last = lastItem();
			out->prev = last.offset();
			if (last.valid()) {
				last->next = out.offset();
			} else {
				oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "NodeBuffer malloc failed due to invalid last element pointer.";
				return nullptr;
			}
			m_header.bytesUsed += out.size();
		} else {
			oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "Unknown";
		}
		return out;
	}
	oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "Insufficient space:" << fullSize << "needed," << available() << "available";
	return nullptr;
}

template<typename size_t, typename Item>
Error NodeBuffer<size_t, Item>::free(ItemPtr item) {
	auto prev = this->prev(item);
	auto next = this->next(item);
	if (prev.valid() && next.valid()) {
		prev->next = next.offset();
		next->prev = prev.offset();
	} else {
		if (!prev.valid()) {
			oxTrace("ox::ptrarith::NodeBuffer::free::fail") << "NodeBuffer free failed due to invalid prev element pointer:" << prev.offset();
		}
		if (!next.valid()) {
			oxTrace("ox::ptrarith::NodeBuffer::free::fail") << "NodeBuffer free failed due to invalid next element pointer:" << next.offset();
		}
		return OxError(1);
	}
	m_header.bytesUsed -= item.size();
	return OxError(0);
}

template<typename size_t, typename Item>
Error NodeBuffer<size_t, Item>::setSize(size_t size) {
	auto last = lastItem();
	if ((last.valid() && last.end() >= size) || size < sizeof(m_header)) {
		return OxError(1);
	} else {
		m_header.size = size;
		return OxError(0);
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
void NodeBuffer<size_t, Item>::truncate() {
	m_header.size = m_header.bytesUsed;
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
		size_t size() const {
			return m_size;
		}

		void setSize(size_t size) {
			m_size = size;
		}
};

}
