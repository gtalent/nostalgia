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
		uint8_t *m_dataEnd = nullptr;
		size_t m_itemOffset = 0;
		size_t m_itemSize = 0;

	public:
		inline Ptr() = default;

		inline Ptr(void *dataStart, void *dataEnd, size_t itemOffset, size_t itemSize = sizeof(T)) {
			// do some sanity checks before assuming this is valid
			if (itemSize >= sizeof(T) and
			    m_dataStart and
			    m_dataStart < m_dataEnd and
			    m_dataStart + m_itemOffset + m_itemSize < m_dataEnd) {
				m_dataStart = static_cast<uint8_t*>(dataStart);
				m_dataEnd = static_cast<uint8_t*>(dataEnd);
				m_itemOffset = itemOffset;
				m_itemSize = itemSize;
			}
		}

		inline Ptr(void *dataStart, void *dataEnd, void *item, size_t itemSize = sizeof(T)) {
			Ptr(dataStart, dataEnd, reinterpret_cast<size_t>(static_cast<uint8_t*>(item) - static_cast<uint8_t*>(dataStart)), itemSize);
		}

		inline bool valid() const;

		inline size_t size() const;

		inline size_t offset() const;

		inline T *operator->() const;

		inline operator T*() const;

		inline operator size_t() const;

		inline T &operator*() const;

		inline Ptr &operator=(size_t offset);

		inline Ptr &operator+=(size_t offset);

};

template<typename T, typename size_t>
inline bool Ptr<T, size_t>::valid() const {
	return m_dataStart and m_itemOffset;
}

template<typename T, typename size_t>
inline size_t Ptr<T, size_t>::size() const {
	return m_itemSize;
}

template<typename T, typename size_t>
inline size_t Ptr<T, size_t>::offset() const {
	return m_itemOffset;
}

template<typename T, typename size_t>
inline T *Ptr<T, size_t>::operator->() const {
	return (T*) m_dataStart + m_itemOffset;
}

template<typename T, typename size_t>
inline Ptr<T, size_t>::operator T*() const {
	return (T*) m_dataStart + m_itemOffset;
}

template<typename T, typename size_t>
inline Ptr<T, size_t>::operator size_t() const {
	if (valid()) {
		return m_itemOffset;
	}
	return 0;
}

template<typename T, typename size_t>
inline T &Ptr<T, size_t>::operator*() const {
	return *static_cast<T>(this);
}

template<typename T, typename size_t>
inline Ptr<T, size_t> &Ptr<T, size_t>::operator=(size_t offset) {
	m_itemOffset = offset;
	return *this;
}

template<typename T, typename size_t>
inline Ptr<T, size_t> &Ptr<T, size_t>::operator+=(size_t offset) {
	m_itemOffset += offset;
	return *this;
}

}
}
