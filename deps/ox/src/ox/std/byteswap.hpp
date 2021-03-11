/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "defines.hpp"
#include "stddef.hpp"
#include "types.hpp"
#include "typetraits.hpp"

namespace ox {

template<typename T>
[[nodiscard]] constexpr inline T byteSwap(typename enable_if<sizeof(T) == 1, T>::type i) noexcept {
	return i;
}

template<typename T>
[[nodiscard]] constexpr inline T byteSwap(typename enable_if<sizeof(T) == 2, T>::type i) noexcept {
	return (i << 8) | (i >> 8);
}

template<typename T>
[[nodiscard]] constexpr inline T byteSwap(typename enable_if<sizeof(T) == 4, T>::type i) noexcept {
	return ((i >> 24) & 0x000000ff) |
	       ((i >>  8) & 0x0000ff00) |
	       ((i <<  8) & 0x00ff0000) |
	       ((i << 24) & 0xff000000);
}

template<typename T>
[[nodiscard]] constexpr inline T byteSwap(typename enable_if<sizeof(T) == 8, T>::type i) noexcept {
	return ((i >> 56) & 0x00000000000000ff) |
	       ((i >> 40) & 0x000000000000ff00) |
	       ((i >> 24) & 0x0000000000ff0000) |
	       ((i >>  8) & 0x00000000ff000000) |
	       ((i <<  8) & 0x000000ff00000000) |
	       ((i << 24) & 0x0000ff0000000000) |
	       ((i << 40) & 0x00ff000000000000) |
	       ((i << 56) & 0xff00000000000000);
}


/**
 * Takes an int and byte swaps if the platform is the given condition is true.
 */
template<typename T, bool byteSwap>
[[nodiscard]] constexpr inline T conditionalByteSwap(T i) noexcept {
	if constexpr(byteSwap) {
		return ox::byteSwap<T>(i);
	} else {
		return i;
	}
}

template<typename T, bool byteSwap>
class OX_PACKED ByteSwapInteger {
	private:
		T m_value;

	public:
		constexpr inline ByteSwapInteger() noexcept = default;

		constexpr inline ByteSwapInteger(const ByteSwapInteger &other) noexcept {
			m_value = other.m_value;
		}

		constexpr inline ByteSwapInteger(T value) noexcept: m_value(ox::conditionalByteSwap<T, byteSwap>(value)) {
		}

		constexpr inline const ByteSwapInteger &operator=(const ByteSwapInteger &other) noexcept {
			m_value = other.m_value;
			return *this;
		}

		template<typename I>
		constexpr inline T operator=(I value) noexcept {
			m_value = ox::conditionalByteSwap<T, byteSwap>(value);
			return value;
		}

		constexpr inline operator T() const noexcept {
			return ox::conditionalByteSwap<T, byteSwap>(m_value);
		}

		template<typename I>
		constexpr inline T operator+=(I other) noexcept {
			auto newVal = *this + other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator-=(I other) noexcept {
			auto newVal = *this - other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator*=(I other) noexcept {
			auto newVal = *this * other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator/=(I other) noexcept {
			auto newVal = *this / other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		// Prefix increment
		constexpr inline T operator++() noexcept {
			return operator+=(1);
		}

		// Postfix increment
		constexpr inline T operator++(int) noexcept {
			auto old = *this;
			++*this;
			return old;
		}

		// Prefix decrement
		constexpr inline T operator--() noexcept {
			return operator-=(1);
		}

		// Postfix decrement
		constexpr inline T operator--(int) noexcept {
			auto old = *this;
			--*this;
			return old;
		}

		template<typename I>
		constexpr inline T operator&=(I other) noexcept {
			auto newVal = *this & other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator|=(I other) noexcept {
			auto newVal = *this | other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator^=(I other) noexcept {
			auto newVal = *this ^ other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator>>=(I other) noexcept {
			auto newVal = *this >> other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator<<=(I other) noexcept {
			auto newVal = *this << other;
			m_value = ox::conditionalByteSwap<T, byteSwap>(newVal);
			return newVal;
		}

		[[nodiscard]] constexpr auto get() const noexcept -> T {
			return static_cast<T>(*this);
		}

		/**
		 * Returns the integer as it is stored. If it is stored as little endian,
		 * a little endian integer is returned regardless of the endianness of
		 * the system.
		 */
		[[nodiscard]] constexpr auto raw() const noexcept -> T {
			return m_value;
		}

};

template<typename T>
using LittleEndian = ByteSwapInteger<T, ox::defines::BigEndian>;

template<typename T>
using BigEndian = ByteSwapInteger<T, ox::defines::LittleEndian>;

}
