/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "strops.hpp"
#include "vector.hpp"

namespace ox {

template<typename K, typename T>
class HashMap {

	using key_t = K;
	using value_t = T;

	private:
		struct Pair {
			K key = {};
			T value = {};
		};
		Vector<K> m_keys;
		Vector<Pair*> m_pairs;

	public:
		explicit HashMap(std::size_t size = 100);

		HashMap(HashMap &other);

		~HashMap();

		bool operator==(const HashMap &other) const;

		HashMap &operator=(HashMap &other);

		/**
		 * K is assumed to be a null terminated string.
		 */
		T &operator[](K key);

		/**
		 * K is assumed to be a null terminated string.
		 */
		Result<T*> at(K key);

		/**
		 * K is assumed to be a null terminated string.
		 */
		Result<const T*> at(K key) const;

		bool contains(K key) const;

		std::size_t size() const noexcept;

		const Vector<K> &keys() const noexcept;

	private:
		void expand();

		/**
		 * K is assumed to be a null terminated string.
		 */
		static uint64_t hash(K, int len = 0xFFFF) noexcept;

		/**
		 * K is assumed to be a null terminated string.
		 */
		Pair *const&access(const Vector<Pair*> &pairs, K key) const;

		/**
		 * K is assumed to be a null terminated string.
		 */
		Pair *&access(Vector<Pair*> &pairs, K key);

};

template<typename K, typename T>
HashMap<K, T>::HashMap(std::size_t size): m_pairs(size) {
}

template<typename K, typename T>
HashMap<K, T>::HashMap(HashMap<K, T> &other) {
	m_pairs = other.m_pairs;
}

template<typename K, typename T>
HashMap<K, T>::~HashMap() {
	for (std::size_t i = 0; i < m_pairs.size(); i++) {
		delete m_pairs[i];
	}
}

template<typename K, typename T>
bool HashMap<K, T>::operator==(const HashMap &other) const {
	if (m_keys != other.m_keys) {
		return false;
	}
	for (int i = 0; i < m_keys.size(); i++) {
		auto &k = m_keys[i];
		if (at(k) != other.at(k)) {
			return false;
		}
	}
	return true;
}

template<typename K, typename T>
HashMap<K, T> &HashMap<K, T>::operator=(HashMap<K, T> &other) {
	this->~HashMap<K, T>();
	m_keys = other.m_keys;
	m_pairs = other.m_pairs;
	return *this;
}

template<typename K, typename T>
T &HashMap<K, T>::operator[](K k) {
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
Result<T*> HashMap<K, T>::at(K k) {
	auto p = access(m_pairs, k);
	if (!p) {
		return {nullptr, OxError(1, "Value not found for key")};
	}
	return &p->value;
}

template<typename K, typename T>
Result<const T*> HashMap<K, T>::at(K k) const {
	auto p = access(m_pairs, k);
	if (!p) {
		return OxError(1, "Value not found for key");
	}
	return &p->value;
}

template<typename K, typename T>
bool HashMap<K, T>::contains(K k) const {
	return access(m_pairs, k) != nullptr;
}

template<typename K, typename T>
std::size_t HashMap<K, T>::size() const noexcept {
	return m_keys.size();
}

template<typename K, typename T>
const Vector<K> &HashMap<K, T>::keys() const noexcept {
	return m_keys;
}

template<typename K, typename T>
void HashMap<K, T>::expand() {
	Vector<Pair*> r;
	for (std::size_t i = 0; i < m_keys.size(); i++) {
		auto k = m_keys[i];
		access(r, k) = access(m_pairs, k);
	}
	m_pairs = r;
}

template<typename K, typename T>
uint64_t HashMap<K, T>::hash(K k, int len) noexcept {
	uint64_t sum = 1;
	for (int i = 0; i < len && k[i]; i++) {
		sum += ((sum + k[i]) * 7) * sum;
	}
	return sum;
}

template<typename K, typename T>
typename HashMap<K, T>::Pair *const&HashMap<K, T>::access(const Vector<Pair*> &pairs, K k) const {
	auto h = hash(k) % pairs.size();
	auto hashStr = reinterpret_cast<char*>(&h);
	while (1) {
		const auto &p = pairs[h];
		if (p == nullptr || ox_strcmp(p->key, k) == 0) {
			return p;
		} else {
			h = hash(hashStr, 8) % pairs.size();
		}
	}
}

template<typename K, typename T>
typename HashMap<K, T>::Pair *&HashMap<K, T>::access(Vector<Pair*> &pairs, K k) {
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
