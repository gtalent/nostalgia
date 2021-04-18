/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>

namespace ox::ptrarith {

template<typename T, typename size_t, size_t minOffset = 1>
class [[nodiscard]] Ptr {

	private:
		uint8_t *m_dataStart = nullptr;
		size_t m_dataSize = 0;
		size_t m_itemOffset = 0;
		size_t m_itemSize = 0;
		// this should be removed later on, but the excessive validation is
		// desirable during during heavy development
		mutable uint8_t m_validated = false;

	public:
		constexpr Ptr() noexcept = default;

		constexpr Ptr(std::nullptr_t) noexcept;

		constexpr Ptr(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize = sizeof(T), size_t itemTypeSize = sizeof(T)) noexcept;

		[[nodiscard]]
		constexpr bool valid() const noexcept;

		constexpr size_t size() const noexcept;

		constexpr size_t offset() const noexcept;

		constexpr size_t end() noexcept;

		constexpr const T *get() const noexcept;

		constexpr T *get() noexcept;

		constexpr const T *operator->() const noexcept;

		constexpr T *operator->() noexcept;

		constexpr operator const T*() const noexcept;

		constexpr operator T*() noexcept;

		constexpr const T &operator*() const noexcept;

		constexpr T &operator*() noexcept;

		constexpr operator size_t() const noexcept;

		constexpr bool operator==(const Ptr<T, size_t, minOffset> &other) const noexcept;

		constexpr bool operator!=(const Ptr<T, size_t, minOffset> &other) const noexcept;

		template<typename SubT>
		constexpr const Ptr<SubT, size_t, sizeof(T)> subPtr(size_t offset, size_t size) const noexcept;

		template<typename SubT>
		constexpr const Ptr<SubT, size_t, sizeof(T)> subPtr(size_t offset) const noexcept;

		template<typename SubT>
		constexpr Ptr<SubT, size_t, sizeof(T)> subPtr(size_t offset, size_t size) noexcept;

		template<typename SubT>
		constexpr Ptr<SubT, size_t, sizeof(T)> subPtr(size_t offset) noexcept;

		template<typename SubT>
		constexpr const Ptr<SubT, size_t, minOffset> to() const noexcept;

};

template<typename T, typename size_t, size_t minOffset>
constexpr Ptr<T, size_t, minOffset>::Ptr(std::nullptr_t) noexcept {
}

template<typename T, typename size_t, size_t minOffset>
constexpr Ptr<T, size_t, minOffset>::Ptr(void *dataStart, size_t dataSize, size_t itemStart, size_t itemSize, size_t itemTypeSize) noexcept {
	// do some sanity checks before assuming this is valid
	if (itemSize >= itemTypeSize &&
	    dataStart &&
	    itemStart >= minOffset &&
	    itemStart + itemSize <= dataSize) {
		m_dataStart = reinterpret_cast<uint8_t*>(dataStart);
		m_dataSize = dataSize;
		m_itemOffset = itemStart;
		m_itemSize = itemSize;
	}
}

template<typename T, typename size_t, size_t minOffset>
constexpr bool Ptr<T, size_t, minOffset>::valid() const noexcept {
	m_validated = m_dataStart != nullptr;
	return m_validated;
}

template<typename T, typename size_t, size_t minOffset>
constexpr size_t Ptr<T, size_t, minOffset>::size() const noexcept {
	return m_itemSize;
}

template<typename T, typename size_t, size_t minOffset>
constexpr size_t Ptr<T, size_t, minOffset>::offset() const noexcept {
	return m_itemOffset;
}

template<typename T, typename size_t, size_t minOffset>
constexpr size_t Ptr<T, size_t, minOffset>::end() noexcept {
	return m_itemOffset + m_itemSize;
}

template<typename T, typename size_t, size_t minOffset>
constexpr const T *Ptr<T, size_t, minOffset>::get() const noexcept {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::get())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::get())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr T *Ptr<T, size_t, minOffset>::get() noexcept {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::get())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::get())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr const T *Ptr<T, size_t, minOffset>::operator->() const noexcept {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::operator->())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator->())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr T *Ptr<T, size_t, minOffset>::operator->() noexcept {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::operator->())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator->())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr Ptr<T, size_t, minOffset>::operator const T*() const noexcept {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::operator const T*())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator const T*())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr Ptr<T, size_t, minOffset>::operator T*() noexcept {
	oxAssert(m_validated, "Unvalidated pointer access. (ox::fs::Ptr::operator T*())");
	oxAssert(valid(), "Invalid pointer access. (ox::fs::Ptr::operator T*())");
	return reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr const T &Ptr<T, size_t, minOffset>::operator*() const noexcept {
	oxAssert(m_validated, "Unvalidated pointer dereference. (ox::fs::Ptr::operator*())");
	oxAssert(valid(), "Invalid pointer dereference. (ox::fs::Ptr::operator*())");
	return *reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr T &Ptr<T, size_t, minOffset>::operator*() noexcept {
	oxAssert(m_validated, "Unvalidated pointer dereference. (ox::fs::Ptr::operator*())");
	oxAssert(valid(), "Invalid pointer dereference. (ox::fs::Ptr::operator*())");
	return *reinterpret_cast<T*>(m_dataStart + m_itemOffset);
}

template<typename T, typename size_t, size_t minOffset>
constexpr Ptr<T, size_t, minOffset>::operator size_t() const noexcept {
	if (m_dataStart && m_itemOffset) {
		return m_itemOffset;
	}
	return 0;
}

template<typename T, typename size_t, size_t minOffset>
constexpr bool Ptr<T, size_t, minOffset>::operator==(const Ptr<T, size_t, minOffset> &other) const noexcept {
	return m_dataStart == other.m_dataStart &&
	       m_itemOffset == other.m_itemOffset &&
	       m_itemSize == other.m_itemSize;
}

template<typename T, typename size_t, size_t minOffset>
constexpr bool Ptr<T, size_t, minOffset>::operator!=(const Ptr<T, size_t, minOffset> &other) const noexcept {
	return m_dataStart != other.m_dataStart ||
	       m_itemOffset != other.m_itemOffset ||
	       m_itemSize != other.m_itemSize;
}

template<typename T, typename size_t, size_t minOffset>
template<typename SubT>
constexpr const Ptr<SubT, size_t, sizeof(T)> Ptr<T, size_t, minOffset>::subPtr(size_t offset, size_t size) const noexcept {
	return Ptr<SubT, size_t, sizeof(T)>(get(), this->size(), offset, size);
}

template<typename T, typename size_t, size_t minOffset>
template<typename SubT>
constexpr const Ptr<SubT, size_t, sizeof(T)> Ptr<T, size_t, minOffset>::subPtr(size_t offset) const noexcept {
	oxTrace("ox::fs::Ptr::subPtr") << m_itemOffset << this->size() << offset << m_itemSize << (m_itemSize - offset);
	return subPtr<SubT>(offset, m_itemSize - offset);
}

template<typename T, typename size_t, size_t minOffset>
template<typename SubT>
constexpr Ptr<SubT, size_t, sizeof(T)> Ptr<T, size_t, minOffset>::subPtr(size_t offset, size_t size) noexcept {
	return Ptr<SubT, size_t, sizeof(T)>(get(), this->size(), offset, size);
}

template<typename T, typename size_t, size_t minOffset>
template<typename SubT>
constexpr Ptr<SubT, size_t, sizeof(T)> Ptr<T, size_t, minOffset>::subPtr(size_t offset) noexcept {
	oxTrace("ox::fs::Ptr::subPtr") << m_itemOffset << this->size() << offset << m_itemSize << (m_itemSize - offset);
	return subPtr<SubT>(offset, m_itemSize - offset);
}

template<typename T, typename size_t, size_t minOffset>
template<typename SubT>
constexpr const Ptr<SubT, size_t, minOffset> Ptr<T, size_t, minOffset>::to() const noexcept {
	return Ptr<SubT, size_t, minOffset>(m_dataStart, m_dataSize, m_itemOffset, m_itemSize);
}

}
