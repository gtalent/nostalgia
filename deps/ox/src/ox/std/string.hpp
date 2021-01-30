/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

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

		String(std::size_t cap) noexcept;

		String(const char *str) noexcept;

		String(const char *str, std::size_t size) noexcept;

		String(const String&) noexcept;

		String(String&&) noexcept;

		const String &operator=(const char *str) noexcept;

		const String &operator=(char *str) noexcept;

		const String &operator=(int64_t i) noexcept;

		const String &operator=(const String &src) noexcept;

		const String &operator=(const String &&src) noexcept;

		const String &operator+=(const char *str) noexcept;

		const String &operator+=(char *str) noexcept;

		const String &operator+=(int64_t i) noexcept;

		const String &operator+=(const String &src) noexcept;

		const String operator+(const char *str) const noexcept;

		const String operator+(char *str) const noexcept;

		const String operator+(int64_t i) const noexcept;

		const String operator+(const String &src) const noexcept;

		bool operator==(const String &other) const noexcept;

		bool operator!=(const String &other) const noexcept;

		char operator[](std::size_t i) const noexcept;

		char &operator[](std::size_t i) noexcept;

		char *data() noexcept;

		const char *c_str() const noexcept;

		/**
		 * Returns the number of characters in this string.
		 */
		std::size_t len() const noexcept;

		/**
		 * Returns the number of bytes used for this string.
		 */
		std::size_t bytes() const noexcept;

};

}
