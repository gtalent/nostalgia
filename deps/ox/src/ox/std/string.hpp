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

class String {
	private:
		Buffer m_buff;

	public:
		String() noexcept;

		explicit String(std::size_t cap) noexcept;

		String(const char *str) noexcept;

		String(const char *str, std::size_t size) noexcept;

		String(const String&) noexcept;

		String(String&&) noexcept;

		String &operator=(const char *str) noexcept;

		String &operator=(char *str) noexcept;

		String &operator=(char c) noexcept;

		String &operator=(int i) noexcept;

		String &operator=(int64_t i) noexcept;

		String &operator=(const String &src) noexcept;

		String &operator=(String &&src) noexcept;

		String &operator+=(const char *str) noexcept;

		String &operator+=(char *str) noexcept;

		String &operator+=(char c) noexcept;

		String &operator+=(int i) noexcept;

		String &operator+=(int64_t i) noexcept;

		String &operator+=(const String &src) noexcept;

		String operator+(const char *str) const noexcept;

		String operator+(char *str) const noexcept;

		String operator+(char c) const noexcept;

		String operator+(int i) const noexcept;

		String operator+(int64_t i) const noexcept;

		String operator+(const String &src) const noexcept;

		bool operator==(const String &other) const noexcept;

		bool operator!=(const String &other) const noexcept;

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
		String substr(std::size_t pos) const noexcept;

		[[nodiscard]]
		bool endsWith(const char *other) const noexcept;

		[[nodiscard]]
		bool endsWith(const String &other) const noexcept;

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

constexpr std::size_t String::len() const noexcept {
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

}
