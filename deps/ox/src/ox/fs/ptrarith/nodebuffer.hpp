/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/stddef.hpp>
#include <ox/std/trace.hpp>

#include "ptr.hpp"

namespace ox::ptrarith {

template<typename size_t, typename Item>
class OX_PACKED NodeBuffer {

	public:
		struct OX_PACKED Header {
			ox::LittleEndian<size_t> size = sizeof(Header); // capacity
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
				Iterator(NodeBuffer *buffer, ItemPtr current) noexcept {
					m_buffer = buffer;
					m_current = current;
					oxTrace("ox::ptrarith::Iterator::start") << current.offset();
				}

				operator const Item*() const noexcept {
					return m_current;
				}

				ItemPtr ptr() noexcept {
					return m_current;
				}

				[[nodiscard]]
				Item *get() noexcept {
					return m_current;
				}

				operator ItemPtr() noexcept {
					return m_current;
				}

				operator Item*() noexcept {
					return m_current;
				}

				const Item *operator->() const noexcept {
					return m_current;
				}

				Item *operator->() noexcept {
					return m_current;
				}

				[[nodiscard]]
				bool valid() const noexcept {
					return m_current.valid();
				}

				[[nodiscard]]
				bool hasNext() noexcept {
					if (m_current.valid()) {
						oxTrace("ox::ptrarith::NodeBuffer::Iterator::hasNext::current") << m_current.offset();
						auto next = m_buffer->next(m_current);
						return next.valid() && m_buffer->firstItem() != next;
					}
					return false;
				}

				void next() noexcept {
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
		NodeBuffer() noexcept;

		NodeBuffer(const NodeBuffer &other, size_t size) noexcept;

		explicit NodeBuffer(size_t size) noexcept;

		[[nodiscard]]
		const Iterator iterator() const noexcept;

		[[nodiscard]]
		Iterator iterator() noexcept;

		ItemPtr firstItem() noexcept;

		ItemPtr lastItem() noexcept;

		/**
		 * @return the data section of the given item
		 */
		template<typename T>
		Ptr<T, size_t, sizeof(Item)> dataOf(ItemPtr) noexcept;

		ItemPtr prev(Item *item) noexcept;

		ItemPtr next(Item *item) noexcept;

		/**
		 * Like pointer but omits checks that assume the memory at the offset has
		 * already been initialed as an Item.
		 */
		ItemPtr uninitializedPtr(size_t offset) noexcept;

		ItemPtr ptr(size_t offset) noexcept;

		ItemPtr malloc(size_t size) noexcept;

		Error free(ItemPtr item) noexcept;

		[[nodiscard]]
		bool valid(size_t maxSize) noexcept;

		/**
		 * Set size, capacity.
		 */
		Error setSize(size_t size) noexcept;

		/**
		 * Get size, capacity.
		 * @return capacity
		 */
		[[nodiscard]]
		constexpr size_t size() const noexcept;

		/**
		 * @return the bytes still available in this NodeBuffer
		 */
		[[nodiscard]]
		size_t available() noexcept;

		/**
		 * @return the actual number a bytes need to store the given number of
		 * bytes
		 */
		[[nodiscard]]
		static size_t spaceNeeded(size_t size) noexcept;

		template<typename F>
		Error compact(F cb = [](uint64_t, ItemPtr) {}) noexcept;

	private:
		[[nodiscard]]
		uint8_t *data() noexcept;

};

template<typename size_t, typename Item>
NodeBuffer<size_t, Item>::NodeBuffer(size_t size) noexcept {
	m_header.size = size;
	auto data = reinterpret_cast<uint8_t*>(this) + sizeof(*this);
	ox_memset(data, 0, size - sizeof(*this));
	oxTrace("ox::NodeBuffer::constructor") << m_header.firstItem;
}

template<typename size_t, typename Item>
NodeBuffer<size_t, Item>::NodeBuffer(const NodeBuffer &other, size_t size) noexcept {
	oxTrace("ox::ptrarith::NodeBuffer::copy") << "other.m_header.firstItem:" << other.m_header.firstItem;
	auto data = reinterpret_cast<uint8_t*>(this) + sizeof(*this);
	ox_memset(data, 0, size - sizeof(*this));
	ox_memcpy(this, &other, size);
}

template<typename size_t, typename Item>
const typename NodeBuffer<size_t, Item>::Iterator NodeBuffer<size_t, Item>::iterator() const noexcept {
	return Iterator(this, firstItem());
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::Iterator NodeBuffer<size_t, Item>::iterator() noexcept {
	oxTrace("ox::ptrarith::NodeBuffer::iterator::size") << m_header.size;
	return Iterator(this, firstItem());
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::firstItem() noexcept {
	//oxTrace("ox::ptrarith::NodeBuffer::firstItem") << m_header.firstItem;
	return ptr(m_header.firstItem);
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::lastItem() noexcept {
	auto first = ptr(m_header.firstItem);
	if (first.valid()) {
		return prev(first);
	}
	return nullptr;
}

template<typename size_t, typename Item>
template<typename T>
Ptr<T, size_t, sizeof(Item)> NodeBuffer<size_t, Item>::dataOf(ItemPtr ip) noexcept {
	auto out = ip.template subPtr<T>(sizeof(Item));
	oxAssert(out.size() == ip.size() - sizeof(Item), "Sub Ptr has invalid size.");
	return out;
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::prev(Item *item) noexcept {
	return ptr(item->prev);
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::next(Item *item) noexcept {
	return ptr(item->next);
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::uninitializedPtr(size_t itemOffset) noexcept {
	// make sure this can be read as an Item, and then use Item::size for the size
	std::size_t itemSpace = m_header.size - itemOffset;
	if (itemOffset >= sizeof(Header) &&
	    itemOffset + itemSpace <= size() &&
	    itemSpace >= sizeof(Item)) {
		return ItemPtr(this, m_header.size, itemOffset, itemSpace);
	} else {
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemOffset:" << itemOffset;
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemOffset >= sizeof(Header):" << (itemOffset >= sizeof(Header));
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemSpace >= sizeof(Item):" << (itemSpace >= sizeof(Item));
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemSpace >= item->fullSize():" << (itemSpace >= item->fullSize());
		return ItemPtr(this, m_header.size, 0, 0);
	}
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::ptr(size_t itemOffset) noexcept {
	// make sure this can be read as an Item, and then use Item::size for the size
	std::size_t itemSpace = m_header.size - itemOffset;
	auto item = reinterpret_cast<Item*>(reinterpret_cast<uint8_t*>(this) + itemOffset);
	if (itemOffset >= sizeof(Header) &&
	    itemOffset + itemSpace <= size() &&
	    itemSpace >= sizeof(Item) &&
	    itemSpace >= item->fullSize()) {
		return ItemPtr(this, m_header.size, itemOffset, item->fullSize());
	} else {
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemOffset:" << itemOffset;
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemOffset >= sizeof(Header):" << (itemOffset >= sizeof(Header));
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemSpace >= sizeof(Item):" << (itemSpace >= sizeof(Item));
		//oxTrace("ox::ptrarith::NodeBuffer::ptr::null") << "itemSpace >= item->fullSize():" << (itemSpace >= item->fullSize());
		return ItemPtr(this, m_header.size, 0, 0);
	}
}

template<typename size_t, typename Item>
typename NodeBuffer<size_t, Item>::ItemPtr NodeBuffer<size_t, Item>::malloc(size_t size) noexcept {
	oxTrace("ox::ptrarith::NodeBuffer::malloc") << "Size:" << size;
	size_t fullSize = size + sizeof(Item);
	if (m_header.size - m_header.bytesUsed >= fullSize) {
		auto last = lastItem();
		size_t addr;
		if (last.valid()) {
			addr = last.offset() + last.size();
		} else {
			// there is no first item, so this must be the first item
			if (!m_header.firstItem) {
				oxTrace("ox::ptrarith::NodeBuffer::malloc") << "No first item, initializing.";
				m_header.firstItem = sizeof(m_header);
				addr = m_header.firstItem;
			} else {
				oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "NodeBuffer is in invalid state.";
				return nullptr;
			}
		}
		oxTracef("ox::ptrarith::NodeBuffer::malloc", "buffer size: {}; addr: {}; fullSize: {}", m_header.size.get(), addr, fullSize);
		auto out = ItemPtr(this, m_header.size, addr, fullSize);
		if (!out.valid()) {
			oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "Unknown";
			return nullptr;
		}
		ox_memset(out, 0, fullSize);
		new (out) Item;
		out->setSize(size);

		auto first = firstItem();
		auto oldLast = last;
		out->next = first.offset();
		if (first.valid()) {
			first->prev = out.offset();
		} else {
			oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "NodeBuffer malloc failed due to invalid first element pointer.";
			return nullptr;
		}

		if (oldLast.valid()) {
			out->prev = oldLast.offset();
			oldLast->next = out.offset();
		} else { // check to see if this is the first allocation
			if (out.offset() != first.offset()) {
				// if this is not the first allocation, there should be an oldLast
				oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "NodeBuffer malloc failed due to invalid last element pointer.";
				return nullptr;
			}
			out->prev = out.offset();
		}
		m_header.bytesUsed += out.size();
		oxTrace("ox::ptrarith::NodeBuffer::malloc") << "Offset:" << out.offset();
		return out;
	}
	oxTrace("ox::ptrarith::NodeBuffer::malloc::fail") << "Insufficient space:" << fullSize << "needed," << available() << "available";
	return nullptr;
}

template<typename size_t, typename Item>
Error NodeBuffer<size_t, Item>::free(ItemPtr item) noexcept {
	oxTrace("ox::ptrarith::NodeBuffer::free") << "offset:" << item.offset();
	auto prev = this->prev(item);
	auto next = this->next(item);
	if (prev.valid() && next.valid()) {
		if (next != item) {
			prev->next = next;
			next->prev = prev;
			if (item.offset() == m_header.firstItem) {
				m_header.firstItem = next;
			}
		} else {
			// only one item, null out first
			oxTrace("ox::ptrarith::NodeBuffer::free") << "Nulling out firstItem.";
			m_header.firstItem = 0;
		}
	} else {
		if (!prev.valid()) {
			oxTrace("ox::ptrarith::NodeBuffer::free::fail") << "NodeBuffer free failed due to invalid prev element pointer:" << prev.offset();
			return OxError(1);
		}
		if (!next.valid()) {
			oxTrace("ox::ptrarith::NodeBuffer::free::fail") << "NodeBuffer free failed due to invalid next element pointer:" << next.offset();
			return OxError(1);
		}
	}
	m_header.bytesUsed -= item.size();
	return OxError(0);
}

template<typename size_t, typename Item>
Error NodeBuffer<size_t, Item>::setSize(size_t size) noexcept {
	oxTracef("ox::ptrarith::NodeBuffer::setSize", "{} to {}", m_header.size.get(), size);
	auto last = lastItem();
	auto end = last.valid() ? last.end() : sizeof(m_header);
	oxTracef("ox::ptrarith::NodeBuffer::setSize", "end: {}", end);
	if (end > size) {
		// resizing to less than buffer size
		return OxError(1);
	} else {
		m_header.size = size;
		auto data = reinterpret_cast<uint8_t*>(this) + end;
		ox_memset(data, 0, size - end);
		return OxError(0);
	}
}

template<typename size_t, typename Item>
constexpr size_t NodeBuffer<size_t, Item>::size() const noexcept {
	return m_header.size;
}

template<typename size_t, typename Item>
bool NodeBuffer<size_t, Item>::valid(size_t maxSize) noexcept {
	return m_header.size <= maxSize;
}

template<typename size_t, typename Item>
size_t NodeBuffer<size_t, Item>::available() noexcept {
	return m_header.size - m_header.bytesUsed;
}

template<typename size_t, typename Item>
size_t NodeBuffer<size_t, Item>::spaceNeeded(size_t size) noexcept {
	return sizeof(Item) + size;
}

template<typename size_t, typename Item>
template<typename F>
Error NodeBuffer<size_t, Item>::compact(F cb) noexcept {
	auto src = firstItem();
	auto dest = ptr(sizeof(*this));
	while (dest.offset() <= src.offset()) {
		if (!src.valid()) {
			return OxError(1);
		}
		if (!dest.valid()) {
			return OxError(2);
		}
		// move node
		ox_memcpy(dest, src, src->fullSize());
		oxReturnError(cb(src, dest));
		// update surrounding nodes
		auto prev = ptr(dest->prev);
		if (prev.valid()) {
			prev->next = dest;
		}
		auto next = ptr(dest->next);
		if (next.valid()) {
			next->prev = dest;
		}
		// update iterators
		src = ptr(dest->next);
		dest = uninitializedPtr(dest.offset() + dest->fullSize());
	}
	return OxError(0);
}

template<typename size_t, typename Item>
uint8_t *NodeBuffer<size_t, Item>::data() noexcept {
	return reinterpret_cast<uint8_t*>(ptr(sizeof(*this)).get());
}


template<typename size_t>
struct OX_PACKED Item {
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
