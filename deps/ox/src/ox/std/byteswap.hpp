/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/__buildinfo/defines.hpp>

#include "types.hpp"

namespace ox {

constexpr inline int8_t byteSwap(int8_t i) {
	return i;
}

constexpr inline int16_t byteSwap(int16_t i) {
	return (i << 8) | (i >> 8);
}

constexpr inline int32_t byteSwap(int32_t i) {
	return ((i >> 24) & 0x000000ff) |
	       ((i >>  8) & 0x0000ff00) |
	       ((i <<  8) & 0x00ff0000) |
	       ((i << 24) & 0xff000000);
}

constexpr inline int64_t byteSwap(int64_t i) {
	return ((i >> 56) & 0x00000000000000ff) |
	       ((i >> 40) & 0x000000000000ff00) |
	       ((i >> 24) & 0x0000000000ff0000) |
	       ((i >>  8) & 0x00000000ff000000) |
	       ((i <<  8) & 0x000000ff00000000) |
	       ((i << 24) & 0x0000ff0000000000) |
	       ((i << 40) & 0x00ff000000000000) |
	       ((i << 56) & 0xff00000000000000);
}

constexpr inline uint16_t byteSwap(uint8_t i) {
	return i;
}

constexpr inline uint16_t byteSwap(uint16_t i) {
	return (i << 8) | (i >> 8);
}

constexpr inline uint32_t byteSwap(uint32_t i) {
	return ((i >> 24) & 0x000000ff) |
	       ((i >>  8) & 0x0000ff00) |
	       ((i <<  8) & 0x00ff0000) |
	       ((i << 24) & 0xff000000);
}

constexpr inline uint64_t byteSwap(uint64_t i) {
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
 * Takes an int and byte swaps if the platform is big endian.
 */
template<typename T>
constexpr inline T bigEndianAdapt(T i) {
	if constexpr(ox::defines::BigEndian) {
		return byteSwap(i);
	} else {
		return i;
	}
}


template<typename T>
class __attribute__((packed)) LittleEndian {
	private:
		T m_value;

	public:
		constexpr inline LittleEndian() = default;

		constexpr inline LittleEndian(const LittleEndian &other) {
			m_value = other.m_value;
		}

		constexpr inline LittleEndian(T value) {
			m_value = ox::bigEndianAdapt(value);
		}

		constexpr inline const LittleEndian &operator=(const LittleEndian &other) {
			m_value = other.m_value;
			return *this;
		}

		template<typename I>
		constexpr inline T operator=(I value) {
			m_value = ox::bigEndianAdapt(value);
			return value;
		}

		constexpr inline operator T() const {
			return ox::bigEndianAdapt(m_value);
		}

		template<typename I>
		constexpr inline T operator+=(I other) {
			auto newVal = *this + other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator-=(I other) {
			auto newVal = *this - other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator*=(I other) {
			auto newVal = *this * other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator/=(I other) {
			auto newVal = *this / other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		// Prefix increment
		constexpr inline T operator++() {
			return operator+=(1);
		}

		// Postfix increment
		constexpr inline T operator++(int) {
			auto old = *this;
			++*this;
			return old;
		}

		// Prefix decrement
		constexpr inline T operator--() {
			return operator-=(1);
		}

		// Postfix decrement
		constexpr inline T operator--(int) {
			auto old = *this;
			--*this;
			return old;
		}

		template<typename I>
		constexpr inline T operator&=(I other) {
			auto newVal = *this & other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator|=(I other) {
			auto newVal = *this | other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator^=(I other) {
			auto newVal = *this ^ other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator>>=(I other) {
			auto newVal = *this >> other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		template<typename I>
		constexpr inline T operator<<=(I other) {
			auto newVal = *this << other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

};

}
