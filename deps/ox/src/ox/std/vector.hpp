/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"

namespace ox {

template<typename T>
class Vector {

	private:
		size_t m_size = 0;
		T *m_items = nullptr;

	public:
		Vector() = default;

		explicit Vector(size_t size);

		Vector(Vector &other);

		Vector(Vector &&other);

		~Vector();

		size_t size() const;

		void resize(size_t size);

		Vector &operator=(Vector &other);

		Vector &operator=(Vector &&other);

		T &operator[](size_t i);

		const T &operator[](size_t i) const;

};

template<typename T>
Vector<T>::Vector(size_t size) {
	m_size = size;
	m_items = new T[m_size];
}

template<typename T>
Vector<T>::Vector(Vector<T> &other) {
	m_size = size;
	m_items = new T[m_size];
	for (size_t i = 0; i < m_size; i++) {
		m_items[i] = other.m_items[i];
	}
}

template<typename T>
Vector<T>::Vector(Vector<T> &&other) {
	m_size = other.m_size;
	m_items = other.m_items;
	other.m_size = 0;
	other.m_items = nullptr;
}

template<typename T>
Vector<T>::~Vector() {
	if (m_items) {
		delete[] m_items;
		m_items = nullptr;
	}
}

template<typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &other) {
	~Vector<T>();
	m_size = size;
	m_items = new T[m_size];
	for (size_t i = 0; i < m_size; i++) {
		m_items[i] = other.m_items[i];
	}
	return *this;
}

template<typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&other) {
	~Vector<T>();
	m_size = other.m_size;
	m_items = other.m_items;
	other.m_size = 0;
	other.m_items = nullptr;
	return *this;
}

template<typename T>
size_t Vector<T>::size() const {
	return m_size;
};

template<typename T>
void Vector<T>::resize(size_t size) {
	auto oldItems = m_items;
	m_items = new T[size];
	const auto itRange = size > m_size ? m_size : size;
	for (size_t i = 0; i < itRange; i++) {
		m_items[i] = oldItems[i];
	}
	m_size = size;
	delete[] oldItems;
}

template<typename T>
T &Vector<T>::operator[](size_t i) {
	return *(m_items[i]);
}

template<typename T>
const T &Vector<T>::operator[](size_t i) const {
	return *(m_items[i]);
}

}
