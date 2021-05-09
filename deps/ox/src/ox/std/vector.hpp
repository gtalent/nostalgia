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

template<typename T>
class Vector {

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

template<typename T>
Vector<T>::Vector(std::size_t size) noexcept {
	m_size = size;
	m_cap = m_size;
	m_items = bit_cast<T*>(new AllocAlias<T>[m_cap]);
	for (std::size_t i = 0; i < size; i++) {
		m_items[i] = {};
	}
}

template<typename T>
Vector<T>::Vector(const Vector<T> &other) {
	m_size = other.m_size;
	m_cap = other.m_cap;
	m_items = bit_cast<T*>(new AllocAlias<T>[m_cap]);
	for (std::size_t i = 0; i < m_size; i++) {
		m_items[i] = move(other.m_items[i]);
	}
}

template<typename T>
Vector<T>::Vector(Vector<T> &&other) noexcept {
	m_size = other.m_size;
	m_cap = other.m_cap;
	m_items = other.m_items;
	other.m_size = 0;
	other.m_cap = 0;
	other.m_items = nullptr;
}

template<typename T>
Vector<T>::~Vector() {
	clear();
	delete[] bit_cast<AllocAlias<T>*>(m_items);
	m_items = nullptr;
}

template<typename T>
bool Vector<T>::operator==(const Vector<T> &other) const {
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

template<typename T>
constexpr Vector<T> &Vector<T>::operator=(const Vector<T> &other) {
	if (this != &other) {
		clear();
		delete[] bit_cast<AllocAlias<T>*>(m_items);
		m_size = other.m_size;
		m_cap = other.m_cap;
		m_items = bit_cast<T*>(new AllocAlias<T>[m_cap]);
		for (std::size_t i = 0; i < m_size; i++) {
			m_items[i] = other.m_items[i];
		}
	}
	return *this;
}

template<typename T>
constexpr Vector<T> &Vector<T>::operator=(Vector<T> &&other) noexcept {
	if (this != &other) {
		clear();
		delete[] bit_cast<AllocAlias<T>*>(m_items);
		m_size = other.m_size;
		m_cap = other.m_cap;
		m_items = other.m_items;
		other.m_size = 0;
		other.m_cap = 0;
		other.m_items = nullptr;
	}
	return *this;
}

template<typename T>
constexpr T &Vector<T>::operator[](std::size_t i) noexcept {
	return m_items[i];
}

template<typename T>
constexpr const T &Vector<T>::operator[](std::size_t i) const noexcept {
	return m_items[i];
}

template<typename T>
Result<T&> Vector<T>::front() noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[0];
}

template<typename T>
Result<const T&> Vector<T>::front() const noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[0];
}

template<typename T>
Result<T&> Vector<T>::back() noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[m_size - 1];
}

template<typename T>
Result<const T&> Vector<T>::back() const noexcept {
	if (!m_size) {
		AllocAlias<T> v;
		return {*bit_cast<T*>(&v), OxError(1)};
	}
	return m_items[m_size - 1];
}

template<typename T>
constexpr std::size_t Vector<T>::size() const noexcept {
	return m_size;
}

template<typename T>
bool Vector<T>::empty() const noexcept {
	return !m_size;
}

template<typename T>
void Vector<T>::clear() {
	if constexpr(is_class<T>()) {
		for (std::size_t i = 0; i < m_size; ++i) {
			m_items[i].~T();
		}
	}
	m_size = 0;
}

template<typename T>
constexpr void Vector<T>::resize(std::size_t size) {
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

template<typename T>
bool Vector<T>::contains(const T &v) const {
	for (std::size_t i = 0; i < m_size; i++) {
		if (m_items[i] == v) {
			return true;
		}
	}
	return false;
}

template<typename T>
void Vector<T>::insert(std::size_t pos, const T &val) {
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

template<typename T>
template<typename... Args>
void Vector<T>::emplace_back(Args&&... args) {
	if (m_size == m_cap) {
		expandCap(m_cap ? m_cap * 2 : 100);
	}
	new (&m_items[m_size]) T{args...};
	++m_size;
}

template<typename T>
void Vector<T>::push_back(const T &item) {
	if (m_size == m_cap) {
		expandCap(m_cap ? m_cap * 2 : 100);
	}
	new (&m_items[m_size]) T(item);
	++m_size;
}

template<typename T>
void Vector<T>::pop_back() {
	--m_size;
	m_items[m_size].~T();
}

template<typename T>
Error Vector<T>::erase(std::size_t pos) {
	if (pos >= m_size) {
		return OxError(1);
	}
	--m_size;
	for (auto i = pos; i < m_size; ++i) {
		m_items[i] = move(m_items[i + 1]);
	}
	return OxError(0);
}

template<typename T>
Error Vector<T>::unordered_erase(std::size_t pos) {
	if (pos >= m_size) {
		return OxError(1);
	}
	m_size--;
	m_items[pos] = move(m_items[m_size]);
	return OxError(0);
}

template<typename T>
void Vector<T>::expandCap(std::size_t cap) {
	auto oldItems = m_items;
	m_cap = cap;
	m_items = bit_cast<T*>(new AllocAlias<T>[m_cap]);
	if (oldItems) { // move over old items
		const auto itRange = cap > m_size ? m_size : cap;
		for (std::size_t i = 0; i < itRange; i++) {
			m_items[i] = move(oldItems[i]);
		}
		for (std::size_t i = itRange; i < m_cap; i++) {
			new (&m_items[i]) T;
		}
		delete[] bit_cast<AllocAlias<T>*>(oldItems);
	}
}

}
