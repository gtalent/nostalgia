/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
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
template<size_t buffLen>
class bstring {
	private:
		uint8_t m_buff[buffLen];

	public:
		bstring();

		bstring(const char *str);

		const bstring &operator=(const char *str);

		const bstring &operator=(char *str);

		bool operator==(const bstring &other);

		char *data();

		/**
		 * Returns the number of characters in this string.
		 */
		size_t len();

		/**
		 * Returns the number of bytes used for this string.
		 */
		size_t size();

		/**
		 * Returns the capacity of bytes for this string.
		 */
		size_t cap();
};

template<size_t size>
bstring<size>::bstring() {
	m_buff[0] = 0;
}

template<size_t size>
bstring<size>::bstring(const char *str) {
	*this = str;
}

template<size_t size>
const bstring<size> &bstring<size>::operator=(const char *str) {
	size_t strLen = ox_strlen(str) + 1;
	if (cap() < strLen) {
		strLen = cap();
	}
	ox_memcpy(m_buff, str, strLen);
	// make sure last element is a null terminator
	m_buff[cap() - 1] = 0;
	return *this;
}

template<size_t size>
const bstring<size> &bstring<size>::operator=(char *str) {
	return *this = (const char*) str;
}

template<size_t buffLen>
bool bstring<buffLen>::operator==(const bstring<buffLen> &other) {
	bool retval = true;
	size_t i = 0;
	while (i < buffLen && (m_buff[i] || other.m_buff[i])) {
		if (m_buff[i] != other.m_buff[i]) {
			retval = false;
			break;
		}
		i++;
	}
	return retval;
}

template<size_t buffLen>
char *bstring<buffLen>::data() {
	return (char*) m_buff;
}

template<size_t buffLen>
size_t bstring<buffLen>::len() {
	size_t length = 0;
	for (size_t i = 0; i < buffLen; i++) {
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

template<size_t buffLen>
size_t bstring<buffLen>::size() {
	size_t i;
	for (i = 0; i < buffLen && m_buff[i]; i++);
	return i + 1; // add one for null terminator
}

template<size_t buffLen>
size_t bstring<buffLen>::cap() {
	return buffLen;
}

}
