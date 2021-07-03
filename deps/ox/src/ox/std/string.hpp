/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#if defined(OX_USE_STDLIB)
#include <string>
#endif

#include "buffer.hpp"
#include "memops.hpp"
#include "strops.hpp"
#include "typetraits.hpp"
#include "vector.hpp"

namespace ox {

template<std::size_t SmallStringSize = 0>
class BasicString {
	private:
		Vector<char, SmallStringSize> m_buff;

	public:
		constexpr BasicString() noexcept;

		constexpr explicit BasicString(std::size_t cap) noexcept;

		constexpr BasicString(const char *str) noexcept;

		constexpr BasicString(const char *str, std::size_t size) noexcept;

		constexpr BasicString(const BasicString&) noexcept;

		constexpr BasicString(BasicString&&) noexcept;

		BasicString &operator=(const char *str) noexcept;

		BasicString &operator=(char *str) noexcept;

		BasicString &operator=(char c) noexcept;

		BasicString &operator=(int i) noexcept;

		BasicString &operator=(int64_t i) noexcept;

		BasicString &operator=(uint64_t i) noexcept;

		BasicString &operator=(const BasicString &src) noexcept;

		BasicString &operator=(BasicString &&src) noexcept;

		constexpr BasicString &operator+=(const char *str) noexcept;

		constexpr BasicString &operator+=(char *str) noexcept;

		constexpr BasicString &operator+=(char c) noexcept;

		constexpr BasicString &operator+=(int i) noexcept;

		constexpr BasicString &operator+=(int64_t i) noexcept;

		constexpr BasicString &operator+=(uint64_t i) noexcept;

		constexpr BasicString &operator+=(const BasicString &src) noexcept;

		BasicString operator+(const char *str) const noexcept;

		BasicString operator+(char *str) const noexcept;

		BasicString operator+(char c) const noexcept;

		BasicString operator+(int i) const noexcept;

		BasicString operator+(int64_t i) const noexcept;

		BasicString operator+(uint64_t i) const noexcept;

		BasicString operator+(const BasicString &src) const noexcept;

		bool operator==(const BasicString &other) const noexcept;

		bool operator!=(const BasicString &other) const noexcept;

		char operator[](std::size_t i) const noexcept;

		char &operator[](std::size_t i) noexcept;

		constexpr Error append(const char *str, std::size_t strLen) noexcept {
			auto currentLen = len();
			m_buff.resize(m_buff.size() + strLen);
			ox_memcpy(&m_buff[currentLen], str, strLen);
			// make sure last element is a null terminator
			m_buff[currentLen + strLen] = 0;
			// this can't fail, but it returns an Error to match BString::append
			return OxError(0);
		}

		[[nodiscard]]
		BasicString substr(std::size_t pos) const noexcept;

		[[nodiscard]]
		bool endsWith(const char *ending) const noexcept;

		[[nodiscard]]
		bool endsWith(const BasicString &ending) const noexcept;

		[[nodiscard]]
		constexpr const char *data() const noexcept {
			return m_buff.data();
		}

		[[nodiscard]]
		constexpr char *data() noexcept {
			return m_buff.data();
		}

		[[nodiscard]]
		constexpr const char *c_str() const noexcept {
			return static_cast<const char*>(m_buff.data());
		}

#ifdef OX_USE_STDLIB
		[[nodiscard]]
		inline std::string toStdString() const {
			return c_str();
		}
#endif

		/**
		 * Returns the number of characters in this string.
		 */
		[[nodiscard]]
		constexpr std::size_t len() const noexcept;

		/**
		 * Returns the number of bytes used for this string.
		 */
		[[nodiscard]]
		std::size_t bytes() const noexcept;

};

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize>::BasicString() noexcept {
	if (m_buff.size()) {
		m_buff[0] = 0;
	} else {
		m_buff.push_back(0);
	}
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize>::BasicString(std::size_t cap) noexcept {
	m_buff.resize(cap + 1);
	m_buff[0] = 0;
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize>::BasicString(const char *str) noexcept {
	if (m_buff.size()) {
		m_buff[0] = 0;
	} else {
		m_buff.push_back(0);
	}
	*this = str;
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize>::BasicString(const char *str, std::size_t size) noexcept {
	m_buff.resize(size + 1);
	memcpy(m_buff.data(), str, size);
	m_buff[size] = 0;
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize>::BasicString(const BasicString &other) noexcept {
	m_buff = other.m_buff;
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize>::BasicString(BasicString &&other) noexcept {
	m_buff = move(other.m_buff);
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(const char *str) noexcept {
	std::size_t strLen = ox_strlen(str) + 1;
	m_buff.resize(strLen + 1);
	memcpy(m_buff.data(), str, strLen);
	// make sure last element is a null terminator
	m_buff[m_buff.size() - 1] = 0;
	return *this;
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(char *str) noexcept {
	return *this = const_cast<const char*>(str);
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(char c) noexcept {
	char str[] = {c, 0};
	return this->operator=(str);
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(int i) noexcept {
	return this->operator=(static_cast<int64_t>(i));
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(int64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator=(str);
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(uint64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator=(str);
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(const BasicString &src) noexcept {
	return *this = src.c_str();
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator=(BasicString &&src) noexcept {
	m_buff = move(src.m_buff);
	return *this;
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator+=(const char *str) noexcept {
	std::size_t strLen = ox_strlen(str);
	oxIgnoreError(append(str, strLen));
	return *this;
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator+=(char *str) noexcept {
	return *this += static_cast<const char*>(str);
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator+=(char c) noexcept {
	const char str[] = {c, 0};
	return this->operator+=(str);
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator+=(int i) noexcept {
	return this->operator+=(static_cast<int64_t>(i));
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator+=(int64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator+=(str);
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator+=(uint64_t i) noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return this->operator+=(str);
}

template<std::size_t SmallStringSize>
constexpr BasicString<SmallStringSize> &BasicString<SmallStringSize>::operator+=(const BasicString &src) noexcept {
	return *this += src.c_str();
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::operator+(const char *str) const noexcept {
	const std::size_t strLen = ox_strlen(str);
	const auto currentLen = len();
	BasicString<SmallStringSize> cpy(currentLen + strLen);
	cpy.m_buff.resize(m_buff.size() + strLen);
	memcpy(&cpy.m_buff[0], m_buff.data(), currentLen);
	memcpy(&cpy.m_buff[currentLen], str, strLen);
	// make sure last element is a null terminator
	cpy.m_buff[currentLen + strLen] = 0;
	return move(cpy);
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::operator+(char *str) const noexcept {
	return *this + static_cast<const char*>(str);
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::operator+(char c) const noexcept {
	const char str[] = {c, 0};
	return *this + str;
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::operator+(int i) const noexcept {
	return this->operator+(static_cast<int64_t>(i));
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::operator+(int64_t i) const noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return *this + str;
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::operator+(uint64_t i) const noexcept {
	char str[65] = {};
	ox_itoa(i, str);
	return *this + str;
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::operator+(const BasicString &src) const noexcept {
	return *this + src.c_str();
}

template<std::size_t SmallStringSize>
bool BasicString<SmallStringSize>::operator==(const BasicString &other) const noexcept {
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

template<std::size_t SmallStringSize>
bool BasicString<SmallStringSize>::operator!=(const BasicString &other) const noexcept {
	return !operator==(other);
}

template<std::size_t SmallStringSize>
char BasicString<SmallStringSize>::operator[](std::size_t i) const noexcept {
	return m_buff[i];
}

template<std::size_t SmallStringSize>
char &BasicString<SmallStringSize>::operator[](std::size_t i) noexcept {
	return m_buff[i];
}

template<std::size_t SmallStringSize>
BasicString<SmallStringSize> BasicString<SmallStringSize>::substr(std::size_t pos) const noexcept {
	return m_buff.data() + pos;
}

template<std::size_t SmallStringSize>
bool BasicString<SmallStringSize>::endsWith(const char *ending) const noexcept {
	const auto endingLen = ox_strlen(ending);
	return len() >= endingLen && ox_strcmp(data() + (len() - endingLen), ending) == 0;
}

template<std::size_t SmallStringSize>
bool BasicString<SmallStringSize>::endsWith(const BasicString &ending) const noexcept {
	const auto endingLen = ending.len();
	return len() >= endingLen && ox_strcmp(data() + (len() - endingLen), ending.c_str()) == 0;
}

template<std::size_t SmallStringSize>
std::size_t BasicString<SmallStringSize>::bytes() const noexcept {
	std::size_t i;
	for (i = 0; i < m_buff.size() && m_buff[i]; i++);
	return i + 1; // add one for null terminator
}

template<std::size_t SmallStringSize>
constexpr std::size_t BasicString<SmallStringSize>::len() const noexcept {
	std::size_t length = 0;
	for (const auto c : m_buff) {
		auto b = static_cast<uint8_t>(c);
		if (b) {
			if ((b & 128) == 0) { // normal ASCII character
				++length;
			} else if ((b & (256 << 6)) == (256 << 6)) { // start of UTF-8 character
				++length;
			}
		} else {
			break;
		}
	}
	return length;
}

extern template class BasicString<0>;

using String = BasicString<0>;

}
