/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>

namespace ox {
namespace fs {

template<typename T, typename size_t, size_t minOffset = 1>
class Ptr {

	private:
		uint8_t *m_dataStart = nullptr;
		size_t m_itemOffset = 0;
		size_t m_itemSize = 0;
		// this should be removed later on, but the excessive validation is
		// desirable during during heavy development
		mutable uint8_t m_validated = false;

	public:
		inline Ptr() = default;

		inline Ptr(std::nullptr_t);

		inline Ptr(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize = sizeof(T));

		inline bool valid() const;

		inline size_t size() const;

		inline size_t offset() const;

		inline size_t end();

		inline T *get() const;

		inline T *operator->() const;

		inline operator T*() const;

		inline operator size_t() const;

		inline T &operator*() const;

		inline void init(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize);

};

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::Ptr(std::nullptr_t) {
}

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::Ptr(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize) {
	init(dataStart, dataSize, itemStart, itemSize);
}

template<typename T, typename size_t, size_t minOffset>
inline bool Ptr<T, size_t, minOffset>::valid() const {
	m_validated = m_dataStart != nullptr;
	return m_validated;
}

template<typename T, typename size_t, size_t minOffset>
inline size_t Ptr<T, size_t, minOffset>::size() const {
	return m_itemSize;
}

template<typename T, typename size_t, size_t minOffset>
inline size_t Ptr<T, size_t, minOffset>::offset() const {
	return m_itemOffset;
}

template<typename T, typename size_t, size_t minOffset>
inline size_t Ptr<T, size_t, minOffset>::end() {
	return m_itemOffset + m_itemSize;
}

template<typename T, typename size_t, size_t minOffset>
inline T *Ptr<T, size_t, minOffset>::get() const {
	ox_assert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::get())");
	ox_assert(valid(), "Invalid pointer access. (ox::fs::Ptr::get())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline T *Ptr<T, size_t, minOffset>::operator->() const {
	ox_assert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::operator->())");
	ox_assert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator->())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::operator T*() const {
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::operator size_t() const {
	if (m_dataStart and m_itemOffset) {
		return m_itemOffset;
	}
	return 0;
}

template<typename T, typename size_t, size_t minOffset>
inline T &Ptr<T, size_t, minOffset>::operator*() const {
	ox_assert(m_validated, "Unvalidated pointer dereference. (ox::fs::Ptr::operator*())");
	ox_assert(valid(), "Invalid pointer dereference. (ox::fs::Ptr::operator*())");
	return *static_cast<T>(this);
}

template<typename T, typename size_t, size_t minOffset>
inline void Ptr<T, size_t, minOffset>::init(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize) {
	// do some sanity checks before assuming this is valid
	if (itemSize >= sizeof(T) and
	    dataStart and
	    itemStart >= minOffset and
	    itemStart + itemSize <= dataSize) {
		m_dataStart = static_cast<uint8_t*>(dataStart);
		m_itemOffset = itemStart;
		m_itemSize = itemSize;
	}
}

}
}
