/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>

namespace ox::fs {

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

		inline const T *get() const;

		inline T *get();

		inline const T *operator->() const;

		inline T *operator->();

		inline operator const T*() const;

		inline operator T*();

		inline const T &operator*() const;

		inline T &operator*();

		inline operator size_t() const;

		template<typename SubT>
		inline Ptr<SubT, size_t, sizeof(T)> subPtr(size_t offset, size_t size);

		template<typename SubT>
		inline Ptr<SubT, size_t, sizeof(T)> subPtr(size_t offset);

};

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::Ptr(std::nullptr_t) {
}

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::Ptr(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize) {
	// do some sanity checks before assuming this is valid
	if (itemSize >= sizeof(T) and
	    dataStart and
	    itemStart >= minOffset and
	    itemStart + itemSize <= dataSize) {
		m_dataStart = reinterpret_cast<uint8_t*>(dataStart);
		m_itemOffset = itemStart;
		m_itemSize = itemSize;
	}
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
inline const T *Ptr<T, size_t, minOffset>::get() const {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::get())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::get())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline T *Ptr<T, size_t, minOffset>::get() {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::get())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::get())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline const T *Ptr<T, size_t, minOffset>::operator->() const {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::operator->())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator->())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline T *Ptr<T, size_t, minOffset>::operator->() {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::operator->())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator->())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::operator const T*() const {
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::operator T*() {
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
inline const T &Ptr<T, size_t, minOffset>::operator*() const {
	oxAssert(m_validated, "Unvalidated pointer dereference. (ox::fs::Ptr::operator*())");
	oxAssert(valid(), "Invalid pointer dereference. (ox::fs::Ptr::operator*())");
	return *reinterpret_cast<T*>(this);
}

template<typename T, typename size_t, size_t minOffset>
inline T &Ptr<T, size_t, minOffset>::operator*() {
	oxAssert(m_validated, "Unvalidated pointer dereference. (ox::fs::Ptr::operator*())");
	oxAssert(valid(), "Invalid pointer dereference. (ox::fs::Ptr::operator*())");
	return *reinterpret_cast<T*>(this);
}

template<typename T, typename size_t, size_t minOffset>
inline Ptr<T, size_t, minOffset>::operator size_t() const {
	if (m_dataStart and m_itemOffset) {
		return m_itemOffset;
	}
	return 0;
}

template<typename T, typename size_t, size_t minOffset>
template<typename SubT>
inline Ptr<SubT, size_t, sizeof(T)> Ptr<T, size_t, minOffset>::subPtr(size_t offset, size_t size) {
	auto out = Ptr<SubT, size_t, sizeof(T)>(get(), this->size(), offset, size);
	return out;
}

template<typename T, typename size_t, size_t minOffset>
template<typename SubT>
inline Ptr<SubT, size_t, sizeof(T)> Ptr<T, size_t, minOffset>::subPtr(size_t offset) {
	oxTrace("ox::fs::Ptr::subPtr") << m_itemOffset << this->size() << offset << m_itemSize << (m_itemSize - offset);
	return subPtr<SubT>(offset, m_itemSize - offset);
}

}
