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
class larray {

	private:
		size_t m_size = 0;
		T *m_items = nullptr;

	public:
		larray() = default;

		explicit larray(size_t size);

		larray(larray &other);

		larray(larray &&other);

		~larray();

		size_t size() const;

		larray &operator=(larray &other);

		larray &operator=(larray &&other);

		T &operator[](size_t i);

		const T &operator[](size_t i) const;

};

template<typename T>
larray<T>::larray(size_t size) {
	m_size = size;
	m_items = new T[m_size];
}

template<typename T>
larray<T>::larray(larray<T> &other) {
	m_size = size;
	m_items = new T[m_size];
	for (size_t i = 0; i < m_size; i++) {
		m_items[i] = other.m_items[i];
	}
}

template<typename T>
larray<T>::larray(larray<T> &&other) {
	m_size = other.m_size;
	m_items = other.m_items;
	other.m_size = 0;
	other.m_items = nullptr;
}

template<typename T>
larray<T>::~larray() {
	if (m_items) {
		delete m_items;
	}
}

template<typename T>
larray<T> &larray<T>::operator=(larray<T> &other) {
	m_size = size;
	m_items = new T[m_size];
	for (size_t i = 0; i < m_size; i++) {
		m_items[i] = other.m_items[i];
	}
	return *this;
}

template<typename T>
larray<T> &larray<T>::operator=(larray<T> &&other) {
	m_size = other.m_size;
	m_items = other.m_items;
	other.m_size = 0;
	other.m_items = nullptr;
	return *this;
}

template<typename T>
size_t larray<T>::size() const {
	return m_size;
};

template<typename T>
T &larray<T>::operator[](size_t i) {
	return *(m_items[i]);
}

template<typename T>
const T &larray<T>::operator[](size_t i) const {
	return *(m_items[i]);
}

}
