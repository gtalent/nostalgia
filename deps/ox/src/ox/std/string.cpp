/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "string.hpp"
#include "types.hpp"

namespace ox {

String::String() noexcept {
	if (m_buff.size()) {
		m_buff[0] = 0;
	} else {
		m_buff.push_back(0);
	}
}

String::String(std::size_t cap) noexcept {
	m_buff.resize(cap + 1);
	m_buff[0] = 0;
}

String::String(const char *str) noexcept {
	if (m_buff.size()) {
		m_buff[0] = 0;
	} else {
		m_buff.push_back(0);
	}
	*this = str;
}

String::String(const char *str, std::size_t size) noexcept {
	m_buff.resize(size + 1);
	memcpy(m_buff.data(), str, size);
	m_buff[size] = 0;
}

String::String(const String &other) noexcept {
	m_buff = other.m_buff;
}

String::String(String &&other) noexcept {
	m_buff = ox::move(other.m_buff);
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

const String &String::operator=(int64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator=(str);
}

const String &String::operator=(const String &src) noexcept {
	return *this = src.c_str();
}

const String &String::operator=(const String &&src) noexcept {
	m_buff = ox::move(src.m_buff);
	return *this;
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

const String &String::operator+=(const String &src) noexcept {
	return *this += src.c_str();
}

const String String::operator+(const char *str) const noexcept {
	std::size_t strLen = ox_strlen(str);
	auto currentLen = len();
	String cpy(currentLen + strLen);
	cpy.m_buff.resize(m_buff.size() + strLen);
	memcpy(&cpy.m_buff[0], m_buff.data(), currentLen);
	memcpy(&cpy.m_buff[currentLen], str, strLen);
	// make sure last element is a null terminator
	cpy.m_buff[currentLen + strLen] = 0;
	return cpy;
}

const String String::operator+(char *str) const noexcept {
	return *this + static_cast<const char*>(str);
}

const String String::operator+(int64_t i) const noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return *this + str;
}

const String String::operator+(const String &src) const noexcept {
	return *this + src.c_str();
}

bool String::operator==(const String &other) const noexcept {
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

bool String::operator!=(const String &other) const noexcept {
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
