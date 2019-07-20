/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"
#include "utility.hpp"

namespace ox {

template<typename T>
class Vector {

	private:
		std::size_t m_size = 0;
		std::size_t m_cap = 0;
		T *m_items = nullptr;

	public:
		Vector() noexcept = default;

		explicit Vector(std::size_t size) noexcept;

		Vector(Vector &other) noexcept;

		Vector(Vector &&other) noexcept;

		~Vector() noexcept;

		Vector &operator=(Vector &other) noexcept;

		Vector &operator=(Vector &&other) noexcept;

		T &operator[](std::size_t i) noexcept;

		const T &operator[](std::size_t i) const noexcept;

		T &front() noexcept;

		const T &front() const noexcept;

		T &back() noexcept;

		const T &back() const noexcept;

		std::size_t size() const noexcept;

		void resize(std::size_t size) noexcept;

		T *data();

		bool contains(T) const noexcept;

		template<typename... Args>
		void emplace_back(Args&&... args) noexcept;

		void push_back(const T &item) noexcept;

		void pop_back() noexcept;

		/**
		 * Removes an item from the Vector.
		 * @param pos position of item to remove
		 */
		void erase(std::size_t pos) noexcept;

		/**
		 * Moves the last item in the Vector to position pos and decrements the
		 * size by 1.
		 * @param pos position of item to remove
		 */
		void unordered_erase(std::size_t pos) noexcept;

	private:
		void expandCap(std::size_t cap) noexcept;

};

template<typename T>
Vector<T>::Vector(std::size_t size) noexcept {
	m_size = size;
	m_cap = m_size;
	m_items = reinterpret_cast<T*>(new AllocAlias<T>[m_cap]);
	for (std::size_t i = 0; i < size; i++) {
		m_items[i] = {};
	}
}

template<typename T>
Vector<T>::Vector(Vector<T> &other) noexcept {
	m_size = other.m_size;
	m_cap = other.m_cap;
	m_items = reinterpret_cast<T*>(new AllocAlias<T>[m_cap]);
	for (std::size_t i = 0; i < m_size; i++) {
		m_items[i] = ox::move(other.m_items[i]);
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
Vector<T>::~Vector() noexcept {
	if constexpr(ox::is_class<T>()) {
		for (std::size_t i = 0; i < m_size; i++) {
			m_items[i].~T();
		}
	}
	delete[] reinterpret_cast<AllocAlias<T>*>(m_items);
	m_items = nullptr;
}

template<typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &other) noexcept {
	this->~Vector<T>();
	m_size = other.m_size;
	m_cap = other.m_cap;
	m_items = reinterpret_cast<T*>(new AllocAlias<T>[m_cap]);
	for (std::size_t i = 0; i < m_size; i++) {
		m_items[i] = other.m_items[i];
	}
	return *this;
}

template<typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&other) noexcept {
	this->~Vector<T>();
	m_size = other.m_size;
	m_cap = other.m_cap;
	m_items = other.m_items;
	other.m_size = 0;
	other.m_cap = 0;
	other.m_items = nullptr;
	return *this;
}

template<typename T>
T &Vector<T>::operator[](std::size_t i) noexcept {
	return m_items[i];
}

template<typename T>
const T &Vector<T>::operator[](std::size_t i) const noexcept {
	return m_items[i];
}

template<typename T>
T &Vector<T>::front() noexcept {
	return m_items[0];
}

template<typename T>
const T &Vector<T>::front() const noexcept {
	return m_items[0];
}

template<typename T>
T &Vector<T>::back() noexcept {
	return m_items[m_size - 1];
}

template<typename T>
const T &Vector<T>::back() const noexcept {
	return m_items[m_size - 1];
}

template<typename T>
std::size_t Vector<T>::size() const noexcept {
	return m_size;
}

template<typename T>
void Vector<T>::resize(std::size_t size) noexcept {
	if (m_cap < size) {
		expandCap(size);
	}
	for (std::size_t i = m_size; i < size; i++) {
		m_items[i] = {};
	}
	m_size = size;
}

template<typename T>
T *Vector<T>::data() {
	return m_items;
}

template<typename T>
bool Vector<T>::contains(T v) const noexcept {
	for (std::size_t i = 0; i < m_size; i++) {
		if (m_items[i] == v) {
			return true;
		}
	}
	return false;
}

template<typename T>
template<typename... Args>
void Vector<T>::emplace_back(Args&&... args) noexcept {
	if (m_size == m_cap) {
		expandCap(m_cap ? m_cap * 2 : 100);
	}
	new (&m_items[m_size]) T{args...};
	++m_size;
}

template<typename T>
void Vector<T>::push_back(const T &item) noexcept {
	if (m_size == m_cap) {
		expandCap(m_cap ? m_cap * 2 : 100);
	}
	m_items[m_size] = item;
	++m_size;
}

template<typename T>
void Vector<T>::pop_back() noexcept {
	--m_size;
}

template<typename T>
void Vector<T>::erase(std::size_t pos) noexcept {
	m_size--;
	for (auto i = pos; i < m_size; i++) {
		m_items[i] = m_items[i + 1];
	}
}

template<typename T>
void Vector<T>::unordered_erase(std::size_t pos) noexcept {
	m_size--;
	m_items[pos] = m_items[m_size];
}

template<typename T>
void Vector<T>::expandCap(std::size_t cap) noexcept {
	auto oldItems = m_items;
	m_cap = cap;
	m_items = reinterpret_cast<T*>(new AllocAlias<T>[m_cap]);
	if (oldItems) { // move over old items
		const auto itRange = cap > m_size ? m_size : cap;
		for (std::size_t i = 0; i < itRange; i++) {
			m_items[i] = ox::move(oldItems[i]);
		}
		for (std::size_t i = itRange; i < m_cap; i++) {
			new (&m_items[i]) T;
		}
		delete[] reinterpret_cast<AllocAlias<T>*>(m_items);
	}
}

}
