/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "string.hpp"
#include "vector.hpp"

namespace ox {

template<typename K, typename T>
class HashMap {

	private:
		struct Pair {
			K key;
			T value;
		};
		Vector<K> m_keys;
		Vector<Pair*> m_pairs;

	public:
		explicit HashMap(std::size_t size = 100) noexcept;

		HashMap(HashMap &other) noexcept;

		~HashMap() noexcept;

		HashMap &operator=(HashMap &other) noexcept;

		/**
		 * K is assumed to be a null terminated string.
		 */
		T &operator[](K key) noexcept;

		/**
		 * K is assumed to be a null terminated string.
		 */
		T &at(K key) noexcept;

		bool contains(K key) noexcept;

		std::size_t size() const noexcept;

	private:
		void expand() noexcept;

		/**
		 * K is assumed to be a null terminated string.
		 */
		static std::uint64_t hash(K, int len = 0xFFFF) noexcept;

		/**
		 * K is assumed to be a null terminated string.
		 */
		Pair *&access(Vector<Pair*> &pairs, K key) noexcept;

};

template<typename K, typename T>
HashMap<K, T>::HashMap(std::size_t size) noexcept: m_pairs(size) {
}

template<typename K, typename T>
HashMap<K, T>::HashMap(HashMap<K, T> &other) noexcept {
	m_pairs = other.m_pairs;
}

template<typename K, typename T>
HashMap<K, T>::~HashMap() noexcept {
	for (std::size_t i = 0; i < m_pairs.size(); i++) {
		delete m_pairs[i];
	}
}

template<typename K, typename T>
HashMap<K, T> &HashMap<K, T>::operator=(HashMap<K, T> &other) noexcept {
	this->~HashMap<K, T>();
	m_keys = other.m_keys;
	m_pairs = other.m_pairs;
	return *this;
}

template<typename K, typename T>
T &HashMap<K, T>::operator[](K k) noexcept {
	auto &p = access(m_pairs, k);
	if (p == nullptr) {
		if (m_pairs.size() * 0.7 < m_keys.size()) {
			expand();
		}
		p = new Pair;
		p->key = k;
		m_keys.push_back(k);
	}
	return p->value;
}

template<typename K, typename T>
T &HashMap<K, T>::at(K k) noexcept {
	return operator[](k);
}

template<typename K, typename T>
bool HashMap<K, T>::contains(K k) noexcept {
	return access(m_pairs, k) != nullptr;
}

template<typename K, typename T>
std::size_t HashMap<K, T>::size() const noexcept {
	return m_keys.size();
}

template<typename K, typename T>
void HashMap<K, T>::expand() noexcept {
	Vector<Pair*> r;
	for (std::size_t i = 0; i < m_keys.size(); i++) {
		auto k = m_keys[i];
		access(r, k) = access(m_pairs, k);
	}
	m_pairs = r;
}

template<typename K, typename T>
std::uint64_t HashMap<K, T>::hash(K k, int len) noexcept {
	std::uint64_t sum = 1;
	for (int i = 0; i < len && k[i]; i++) {
		sum += ((sum + k[i]) * 7) * sum;
	}
	return sum;
}

template<typename K, typename T>
typename HashMap<K, T>::Pair *&HashMap<K, T>::access(Vector<Pair*> &pairs, K k) noexcept {
	auto h = hash(k) % pairs.size();
	auto hashStr = reinterpret_cast<char*>(&h);
	while (1) {
		auto &p = pairs[h];
		if (p == nullptr || ox_strcmp(p->key, k) == 0) {
			return p;
		} else {
			h = hash(hashStr, 8) % pairs.size();
		}
	}
}

}
