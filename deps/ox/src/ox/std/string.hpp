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

#include "memops.hpp"
#include "strops.hpp"
#include "typetraits.hpp"
#include "vector.hpp"

namespace ox {

class String {
	private:
		Vector<char> m_buff;

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
		std::size_t len() const noexcept;

		/**
		 * Returns the number of bytes used for this string.
		 */
		[[nodiscard]]
		std::size_t bytes() const noexcept;

};

}
