/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "bit.hpp"
#include "iterator.hpp"
#include "math.hpp"
#include "new.hpp"
#include "types.hpp"
#include "utility.hpp"

namespace ox {

namespace detail {

template<typename T, std::size_t Size = 100>
struct SmallVector {
	private:
		AllocAlias<T> m_data[Size] = {};

	protected:
		constexpr void initItems(T **items, std::size_t cap) noexcept {
			if (cap <= Size) {
				*items = bit_cast<T*>(m_data);
			} else {
				*items = bit_cast<T*>(new AllocAlias<T>[cap]);
			}
		}

		constexpr void moveItemsFrom(T **items, SmallVector &src, const std::size_t count, const std::size_t cap) noexcept {
			if (cap <= Size) {
				const auto dstItems = bit_cast<T*>(m_data);
				const auto srcItems = bit_cast<T*>(src.m_data);
				for (auto i = 0u; i < count; ++i) {
					dstItems[i] = move(srcItems[i]);
				}
				*items = bit_cast<T*>(m_data);
			}
		}

		constexpr void clearItems(AllocAlias<T> *items) noexcept {
			if (items != m_data) {
				delete[] items;
			}
		}

};

template<typename T>
struct SmallVector<T, 0> {
	protected:
		constexpr void initItems(T **items, std::size_t cap) noexcept {
			*items = bit_cast<T*>(new AllocAlias<T>[cap]);
		}

		constexpr void moveItemsFrom(T**, SmallVector&, const std::size_t, const std::size_t) noexcept {
		}

		constexpr void clearItems(AllocAlias<T> *items) noexcept {
			delete[] items;
		}

};

}

template<typename T, std::size_t SmallVectorSize = 0>
class Vector: detail::SmallVector<T, SmallVectorSize> {

	public:
		using value_type = T;
		using size_type = std::size_t;

		template<typename RefType, bool reverse = false>
		struct iterator: public std::iterator<std::bidirectional_iterator_tag, T> {
			private:
				T *m_t = nullptr;
				size_type m_offset = 0;
				size_type m_max = 0;

			public:
				constexpr iterator(T *t, size_type offset, size_type max) {
					m_t = t;
					m_offset = offset;
					m_max = max;
				}

				constexpr iterator operator+(size_type s) const noexcept {
					if constexpr(reverse) {
						return iterator(m_t, max<size_type>(m_offset - s, 0), m_max);
					} else {
						return iterator(m_t, min<size_type>(m_offset + s, m_max), m_max);
					}
				}

				constexpr iterator operator-(size_type s) const noexcept {
					if constexpr(reverse) {
						return iterator(m_t, min<size_type>(m_offset + s, m_max), m_max);
					} else {
						return iterator(m_t, max<size_type>(m_offset - s, 0), m_max);
					}
				}

				constexpr iterator &operator+=(size_type s) noexcept {
					if constexpr(reverse) {
						m_offset = max<size_type>(m_offset - s, 0);
					} else {
						m_offset = min(m_offset + s, m_max);
					}
					return *this;
				}

				constexpr iterator &operator-=(size_type s) noexcept {
					if constexpr(reverse) {
						m_offset = min(m_offset + s, m_max);
					} else {
						m_offset = max<size_type>(m_offset - s, 0);
					}
					return *this;
				}

				constexpr iterator &operator++() noexcept {
					return operator+=(1);
				}

				constexpr iterator &operator--() noexcept {
					return operator-=(1);
				}

				constexpr RefType operator*() const noexcept {
					return m_t[m_offset];
				}

				constexpr RefType operator[](size_type s) const noexcept {
					return m_t[s];
				}

				constexpr bool operator<(const iterator &other) const noexcept {
					return m_offset < other.m_offset;
				}

				constexpr bool operator>(const iterator &other) const noexcept {
					return m_offset > other.m_offset;
				}

				constexpr bool operator<=(const iterator &other) const noexcept {
					return m_offset <= other.m_offset;
				}

				constexpr bool operator>=(const iterator &other) const noexcept {
					return m_offset >= other.m_offset;
				}

				constexpr bool operator==(const iterator &other) const noexcept {
					return m_t == other.m_t && m_offset == other.m_offset && m_max == other.m_max;
				}

				constexpr bool operator!=(const iterator &other) const noexcept {
					return m_t != other.m_t || m_offset != other.m_offset || m_max != other.m_max;
				}

		};

	private:
		std::size_t m_size = 0;
		std::size_t m_cap = 0;
		T *m_items = nullptr;

	public:
		constexpr Vector() noexcept = default;

		explicit Vector(std::size_t size) noexcept;

		Vector(const Vector &other);

		Vector(Vector &&other) noexcept;

		~Vector();

		constexpr iterator<T&> begin() const noexcept {
			return iterator<T&>(m_items, 0, m_size);
		}

		constexpr iterator<T&> end() const noexcept {
			return iterator<T&>(m_items, m_size, m_size);
		}

		constexpr iterator<const T&> cbegin() const noexcept {
			return iterator<const T&>(m_items, 0, m_size);
		}

		constexpr iterator<const T&> cend() const noexcept {
			return iterator<const T&>(m_items, m_size, m_size);
		}

		constexpr iterator<T&, true> rbegin() const noexcept {
			return iterator<T&, true>(m_items, 0, m_size);
		}

		constexpr iterator<T&, true> rend() const noexcept {
			return iterator<T&, true>(m_items, m_size, m_size);
		}

		constexpr iterator<const T&, true> crbegin() const noexcept {
			return iterator<const T&, true>(m_items, 0, m_size);
		}

		constexpr iterator<const T&, true> crend() const noexcept {
			return iterator<const T&, true>(m_items, m_size, m_size);
		}

		bool operator==(const Vector &other) const;

		constexpr Vector &operator=(const Vector &other);

		constexpr Vector &operator=(Vector &&other) noexcept;

		constexpr T &operator[](std::size_t i) noexcept;

		constexpr const T &operator[](std::size_t i) const noexcept;

		Result<T&> front() noexcept;

		Result<const T&> front() const noexcept;

		Result<T&> back() noexcept;

		Result<const T&> back() const noexcept;

		[[nodiscard]]
		constexpr std::size_t size() const noexcept;

		[[nodiscard]]
		bool empty() const noexcept;

		void clear();

		constexpr void resize(std::size_t size);

		[[nodiscard]]
		constexpr T *data() noexcept {
			return m_items;
		}

		[[nodiscard]]
		constexpr const T *data() const noexcept {
			return m_items;
		}

		[[nodiscard]]
		bool contains(const T&) const;

		void insert(std::size_t pos, const T &val);

		template<typename... Args>
		void emplace_back(Args&&... args);

		void push_back(const T &item);

		void pop_back();

		/**
		 * Removes an item from the Vector.
		 * @param pos position of item to remove
		 * @return Error if index is out of bounds
		 */
		Error erase(std::size_t pos);

		/**
		 * Moves the last item in the Vector to position pos and decrements the
		 * size by 1.
		 * @param pos position of item to remove
		 * @return Error if index is out of bounds
		 */
		Error unordered_erase(std::size_t pos);

	private:
		void expandCap(std::size_t cap);

};

template<typename T, std::size_t SmallVectorSize>
Vector<T, SmallVectorSize>::Vector(std::size_t size) noexcept {
	m_size = size;
	m_cap = m_size;
	this->initItems(&m_items, m_cap);
	for (std::size_t i = 0; i < size; ++i) {
		m_items[i] = {};
	}
}

template<typename T, std::size_t SmallVectorSize>
Vector<T, SmallVectorSize>::Vector(const Vector &other) {
	m_size = other.m_size;
	m_cap = other.m_cap;
	this->initItems(&m_items, other.m_cap);
	for (std::size_t i = 0; i < m_size; ++i) {
		m_items[i] = move(other.m_items[i]);
	}
}

template<typename T, std::size_t SmallVectorSize>
Vector<T, SmallVectorSize>::Vector(Vector &&other) noexcept {
	m_size = other.m_size;
	m_cap = other.m_cap;
	m_items = other.m_items;
	this->moveItemsFrom(&m_items, other, m_size, m_cap);
	other.m_size = 0;
	other.m_cap = 0;
	other.m_items = nullptr;
}

template<typename T, std::size_t SmallVectorSize>
Vector<T, SmallVectorSize>::~Vector() {
	clear();
	this->clearItems(bit_cast<AllocAlias<T>*>(m_items));
	m_items = nullptr;
}

template<typename T, std::size_t SmallVectorSize>
bool Vector<T, SmallVectorSize>::operator==(const Vector &other) const {
	if (m_size != other.m_size) {
		return false;
	}
	for (std::size_t i = 0; i < m_size; i++) {
		if (!(m_items[i] == other.m_items[i])) {
			return false;
		}
	}
	return true;
}

template<typename T, std::size_t SmallVectorSize>
constexpr Vector<T, SmallVectorSize> &Vector<T, SmallVectorSize>::operator=(const Vector &other) {
	if (this != &other) {
		clear();
		this->clearItems(bit_cast<AllocAlias<T>*>(m_items));
		m_items = nullptr;
		m_size = other.m_size;
		m_cap = other.m_cap;
		this->initItems(&m_items, other.m_cap);
		for (std::size_t i = 0; i < m_size; i++) {
			m_items[i] = other.m_items[i];
		}
	}
	return *this;
}

template<typename T, std::size_t SmallVectorSize>
constexpr Vector<T, SmallVectorSize> &Vector<T, SmallVectorSize>::operator=(Vector &&other) noexcept {
	if (this != &other) {
		clear();
		this->clearItems(bit_cast<AllocAlias<T>*>(m_items));
		m_size = other.m_size;
		m_cap = other.m_cap;
		m_items = other.m_items;
		this->moveItemsFrom(&m_items, other, m_size, m_cap);
		other.m_size = 0;
		other.m_cap = 0;
		other.m_items = nullptr;
	}
	return *this;
}

template<typename T, std::size_t SmallVectorSize>
constexpr T &Vector<T, SmallVectorSize>::operator[](std::size_t i) noexcept {
	return m_items[i];
}

template<typename T, std::size_t SmallVectorSize>
constexpr const T &Vector<T, SmallVectorSize>::operator[](std::size_t i) const noexcept {
	return m_items[i];
}

template<typename T, std::size_t SmallVectorSize>
Result<T&> Vector<T, SmallVectorSize>::front() noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[0];
}

template<typename T, std::size_t SmallVectorSize>
Result<const T&> Vector<T, SmallVectorSize>::front() const noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[0];
}

template<typename T, std::size_t SmallVectorSize>
Result<T&> Vector<T, SmallVectorSize>::back() noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[m_size - 1];
}

template<typename T, std::size_t SmallVectorSize>
Result<const T&> Vector<T, SmallVectorSize>::back() const noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[m_size - 1];
}

template<typename T, std::size_t SmallVectorSize>
constexpr std::size_t Vector<T, SmallVectorSize>::size() const noexcept {
	return m_size;
}

template<typename T, std::size_t SmallVectorSize>
bool Vector<T, SmallVectorSize>::empty() const noexcept {
	return !m_size;
}

template<typename T, std::size_t SmallVectorSize>
void Vector<T, SmallVectorSize>::clear() {
	if constexpr(is_class<T>()) {
		for (std::size_t i = 0; i < m_size; ++i) {
			m_items[i].~T();
		}
	}
	m_size = 0;
}

template<typename T, std::size_t SmallVectorSize>
constexpr void Vector<T, SmallVectorSize>::resize(std::size_t size) {
	if (m_cap < size) {
		expandCap(size);
	}
	if (m_size < size) {
		for (std::size_t i = m_size; i < size; i++) {
			m_items[i] = {};
		}
	} else {
		for (std::size_t i = size; i < m_size; i++) {
			m_items[i].~T();
		}
	}
	m_size = size;
}

template<typename T, std::size_t SmallVectorSize>
bool Vector<T, SmallVectorSize>::contains(const T &v) const {
	for (std::size_t i = 0; i < m_size; i++) {
		if (m_items[i] == v) {
			return true;
		}
	}
	return false;
}

template<typename T, std::size_t SmallVectorSize>
void Vector<T, SmallVectorSize>::insert(std::size_t pos, const T &val) {
	// TODO: insert should ideally have its own expandCap
	if (m_size == m_cap) {
		expandCap(m_cap ? m_cap * 2 : 100);
	}
	for (auto i = m_size; i > pos; --i) {
		m_items[i] = move(m_items[i - 1]);
	}
	m_items[pos] = val;
	++m_size;
}

template<typename T, std::size_t SmallVectorSize>
template<typename... Args>
void Vector<T, SmallVectorSize>::emplace_back(Args&&... args) {
	if (m_size == m_cap) {
		expandCap(m_cap ? m_cap * 2 : 100);
	}
	new (&m_items[m_size]) T{args...};
	++m_size;
}

template<typename T, std::size_t SmallVectorSize>
void Vector<T, SmallVectorSize>::push_back(const T &item) {
	if (m_size == m_cap) {
		expandCap(m_cap ? m_cap * 2 : 100);
	}
	new (&m_items[m_size]) T(item);
	++m_size;
}

template<typename T, std::size_t SmallVectorSize>
void Vector<T, SmallVectorSize>::pop_back() {
	--m_size;
	m_items[m_size].~T();
}

template<typename T, std::size_t SmallVectorSize>
Error Vector<T, SmallVectorSize>::erase(std::size_t pos) {
	if (pos >= m_size) {
		return OxError(1);
	}
	--m_size;
	for (auto i = pos; i < m_size; ++i) {
		m_items[i] = move(m_items[i + 1]);
	}
	return OxError(0);
}

template<typename T, std::size_t SmallVectorSize>
Error Vector<T, SmallVectorSize>::unordered_erase(std::size_t pos) {
	if (pos >= m_size) {
		return OxError(1);
	}
	--m_size;
	m_items[pos] = move(m_items[m_size]);
	return OxError(0);
}

template<typename T, std::size_t SmallVectorSize>
void Vector<T, SmallVectorSize>::expandCap(std::size_t cap) {
	auto oldItems = m_items;
	m_cap = cap;
	this->initItems(&m_items, cap);
	if (oldItems) { // move over old items
		const auto itRange = cap > m_size ? m_size : cap;
		for (std::size_t i = 0; i < itRange; i++) {
			m_items[i] = move(oldItems[i]);
		}
		for (std::size_t i = itRange; i < m_cap; i++) {
			new (&m_items[i]) T;
		}
		this->clearItems(bit_cast<AllocAlias<T>*>(oldItems));
	}
}

}
