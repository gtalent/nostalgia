/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "string.hpp"

namespace ox {

String::String() noexcept {
	m_buff.push_back(0);
}

String::String(const char *str) noexcept {
	m_buff.push_back(0);
	*this = str;
}

const String &String::operator=(int64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator=(str);
}

const String &String::operator=(const char *str) noexcept {
	std::size_t strLen = ox_strlen(str) + 1;
	m_buff.resize(strLen + 1);
	memcpy(m_buff.data(), str, strLen);
	// make sure last element is a null terminator
	m_buff[m_buff.size() - 1] = 0;
	return *this;
}

const String &String::operator=(char *str) noexcept {
	return *this = static_cast<const char*>(str);
}

const String &String::operator+=(const char *str) noexcept {
	std::size_t strLen = ox_strlen(str);
	auto currentLen = len();
	m_buff.resize(m_buff.size() + strLen);
	memcpy(&m_buff[currentLen], str, strLen);
	// make sure last element is a null terminator
	m_buff[currentLen + strLen] = 0;
	return *this;
}

const String &String::operator+=(char *str) noexcept {
	return *this += static_cast<const char*>(str);
}

const String &String::operator+=(int64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator+=(str);
}

bool String::operator==(const String &other) noexcept {
	bool retval = true;
	std::size_t i = 0;
	while (i < m_buff.size() && (m_buff[i] || other.m_buff[i])) {
		if (m_buff[i] != other.m_buff[i]) {
			retval = false;
			break;
		}
		i++;
	}
	return retval;
}

bool String::operator!=(const String &other) noexcept {
	return !operator==(other);
}

char String::operator[](std::size_t i) const noexcept {
	return m_buff[i];
}

char &String::operator[](std::size_t i) noexcept {
	return m_buff[i];
}

char *String::data() noexcept {
	return m_buff.data();
}

const char *String::c_str() const noexcept {
	return static_cast<const char*>(m_buff.data());
}


std::size_t String::len() const noexcept {
	std::size_t length = 0;
	for (std::size_t i = 0; i < m_buff.size(); i++) {
		uint8_t b = static_cast<uint8_t>(m_buff[i]);
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

std::size_t String::bytes() const noexcept {
	std::size_t i = 0;
	for (i = 0; i < m_buff.size() && m_buff[i]; i++);
	return i + 1; // add one for null terminator
}

}
