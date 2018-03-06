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

template<typename T, typename size_t>
class Ptr {

	private:
		uint8_t *m_dataStart = nullptr;
		size_t m_itemStart = 0;
		size_t m_itemSize = 0;

	public:
		inline Ptr() = default;

		inline Ptr(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize = sizeof(T));

		inline bool valid() const;

		inline size_t size() const;

		inline size_t offset() const;

		inline T *operator->() const;

		inline operator T*() const;

		inline operator size_t() const;

		inline T &operator*() const;

		inline Ptr &operator=(size_t offset);

		inline Ptr &operator+=(size_t offset);

	protected:
		void init(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize);

};

template<typename T, typename size_t>
inline Ptr<T, size_t>::Ptr(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize) {
	init(dataStart, dataSize, itemStart, itemSize);
}

template<typename T, typename size_t>
inline bool Ptr<T, size_t>::valid() const {
	return m_dataStart and m_itemStart;
}

template<typename T, typename size_t>
inline size_t Ptr<T, size_t>::size() const {
	return m_itemSize;
}

template<typename T, typename size_t>
inline size_t Ptr<T, size_t>::offset() const {
	return m_itemStart;
}

template<typename T, typename size_t>
inline T *Ptr<T, size_t>::operator->() const {
	ox_assert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator->())");
	return reinterpret_cast<T*>(m_dataStart + m_itemStart);
}

template<typename T, typename size_t>
inline Ptr<T, size_t>::operator T*() const {
	return reinterpret_cast<T*>(m_dataStart + m_itemStart);
}

template<typename T, typename size_t>
inline Ptr<T, size_t>::operator size_t() const {
	if (valid()) {
		return m_itemStart;
	}
	return 0;
}

template<typename T, typename size_t>
inline T &Ptr<T, size_t>::operator*() const {
	ox_assert(valid(), "Invalid pointer dereference. (ox::fs::Ptr::operator*())");
	return *static_cast<T>(this);
}

template<typename T, typename size_t>
inline Ptr<T, size_t> &Ptr<T, size_t>::operator=(size_t offset) {
	m_itemStart = offset;
	return *this;
}

template<typename T, typename size_t>
inline Ptr<T, size_t> &Ptr<T, size_t>::operator+=(size_t offset) {
	m_itemStart += offset;
	return *this;
}

template<typename T, typename size_t>
void Ptr<T, size_t>::init(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize) {
	// do some sanity checks before assuming this is valid
	if (itemSize >= sizeof(T) and
		 dataStart and
		 itemStart + itemSize <= dataSize) {
		m_dataStart = static_cast<uint8_t*>(dataStart);
		m_itemStart = itemStart;
		m_itemSize = itemSize;
	}
}

}
}
