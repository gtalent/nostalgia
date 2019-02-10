/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "memops.hpp"
#include "strops.hpp"
#include "types.hpp"

namespace ox {

// Bounded String
template<std::size_t buffLen>
class BString {
	private:
		char m_buff[buffLen + 1];

	public:
		constexpr BString() noexcept;

		constexpr BString(const char *str) noexcept;

		constexpr const BString &operator=(const char *str) noexcept;

		constexpr const BString &operator=(char *str) noexcept;

		constexpr const BString &operator=(int64_t i) noexcept;

		constexpr const BString &operator+=(const char *str) noexcept;

		constexpr const BString &operator+=(char *str) noexcept;

		constexpr const BString &operator+=(int64_t i) noexcept;

		constexpr bool operator==(const BString &other) noexcept;

		constexpr char operator[](std::size_t i) const noexcept;

		constexpr char &operator[](std::size_t i) noexcept;

		constexpr char *data() noexcept;

		constexpr const char *c_str() const noexcept;

		/**
		 * Returns the number of characters in this string.
		 */
		std::size_t len() const noexcept;

		/**
		 * Returns the number of bytes used for this string.
		 */
		std::size_t bytes() const noexcept;

		/**
		 * Returns the capacity of bytes for this string.
		 */
		std::size_t cap() const noexcept;
};

template<std::size_t size>
constexpr BString<size>::BString() noexcept {
	m_buff[0] = 0;
}

template<std::size_t size>
constexpr BString<size>::BString(const char *str) noexcept {
	*this = str;
}

template<std::size_t size>
constexpr const BString<size> &BString<size>::operator=(int64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator=(str);
}

template<std::size_t size>
constexpr const BString<size> &BString<size>::operator=(const char *str) noexcept {
	std::size_t strLen = ox_strlen(str) + 1;
	if (cap() < strLen) {
		strLen = cap();
	}
	ox_memcpy(m_buff, str, strLen);
	// make sure last element is a null terminator
	m_buff[cap() - 1] = 0;
	return *this;
}

template<std::size_t size>
constexpr const BString<size> &BString<size>::operator=(char *str) noexcept {
	return *this = (const char*) str;
}

template<std::size_t size>
constexpr const BString<size> &BString<size>::operator+=(const char *str) noexcept {
	std::size_t strLen = ox_strlen(str) + 1;
	auto currentLen = len();
	if (cap() < currentLen + strLen) {
		strLen = cap() - currentLen;
	}
	ox_memcpy(m_buff + currentLen, str, strLen);
	// make sure last element is a null terminator
	m_buff[currentLen + strLen] = 0;
	return *this;
}

template<std::size_t size>
constexpr const BString<size> &BString<size>::operator+=(char *str) noexcept {
	return *this += (const char*) str;
}

template<std::size_t size>
constexpr const BString<size> &BString<size>::operator+=(int64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator+=(str);
}

template<std::size_t buffLen>
constexpr bool BString<buffLen>::operator==(const BString<buffLen> &other) noexcept {
	bool retval = true;
	std::size_t i = 0;
	while (i < buffLen && (m_buff[i] || other.m_buff[i])) {
		if (m_buff[i] != other.m_buff[i]) {
			retval = false;
			break;
		}
		i++;
	}
	return retval;
}

template<std::size_t buffLen>
constexpr char BString<buffLen>::operator[](std::size_t i) const noexcept {
	return m_buff[i];
}

template<std::size_t buffLen>
constexpr char &BString<buffLen>::operator[](std::size_t i) noexcept {
	return m_buff[i];
}

template<std::size_t buffLen>
constexpr char *BString<buffLen>::data() noexcept {
	return (char*) m_buff;
}

template<std::size_t buffLen>
constexpr const char *BString<buffLen>::c_str() const noexcept {
	return (const char*) m_buff;
}


template<std::size_t buffLen>
std::size_t BString<buffLen>::len() const noexcept {
	std::size_t length = 0;
	for (std::size_t i = 0; i < buffLen; i++) {
		uint8_t b = m_buff[i];
		if (b) {
			if ((b & 128) == 0) { // normal ASCII character
				length++;
			} else if ((b & (256 << 6)) == (256 << 6)) { // start of UTF-8 character
				length++;
			}
		} else {
			break;
		}
	}
	return length;
}

template<std::size_t buffLen>
std::size_t BString<buffLen>::bytes() const noexcept {
	std::size_t i;
	for (i = 0; i < buffLen && m_buff[i]; i++);
	return i + 1; // add one for null terminator
}

template<std::size_t buffLen>
std::size_t BString<buffLen>::cap() const noexcept {
	return buffLen;
}

}
