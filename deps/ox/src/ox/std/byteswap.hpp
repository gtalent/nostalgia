/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"

namespace ox {

inline int8_t byteSwap(int8_t i) {
	return i;
}

inline int16_t byteSwap(int16_t i) {
	return (i << 8) | (i >> 8);
}

inline int32_t byteSwap(int32_t i) {
	return ((i >> 24) & 0x000000ff) |
	       ((i >>  8) & 0x0000ff00) |
	       ((i <<  8) & 0x00ff0000) |
	       ((i << 24) & 0xff000000);
}

inline int64_t byteSwap(int64_t i) {
	return ((i >> 56) & 0x00000000000000ff) |
	       ((i >> 40) & 0x000000000000ff00) |
	       ((i >> 24) & 0x0000000000ff0000) |
	       ((i >>  8) & 0x00000000ff000000) |
	       ((i <<  8) & 0x000000ff00000000) |
	       ((i << 24) & 0x0000ff0000000000) |
	       ((i << 40) & 0x00ff000000000000) |
	       ((i << 56) & 0xff00000000000000);
}

inline uint16_t byteSwap(uint8_t i) {
	return i;
}

inline uint16_t byteSwap(uint16_t i) {
	return (i << 8) | (i >> 8);
}

inline uint32_t byteSwap(uint32_t i) {
	return ((i >> 24) & 0x000000ff) |
	       ((i >>  8) & 0x0000ff00) |
	       ((i <<  8) & 0x00ff0000) |
	       ((i << 24) & 0xff000000);
}

inline uint64_t byteSwap(uint64_t i) {
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
inline T bigEndianAdapt(T i) {
#ifdef __BIG_ENDIAN__
	return byteSwap(i);
#else
	return i;
#endif
}


template<typename T>
class __attribute__((packed)) LittleEndian {
	private:
		T m_value;

	public:
		inline LittleEndian() = default;

		inline LittleEndian(T value) {
			m_value = ox::bigEndianAdapt(value);
		}

		inline T operator=(T value) {
			m_value = ox::bigEndianAdapt(value);
			return value;
		}

		inline operator T() const {
			return ox::bigEndianAdapt(m_value);
		}

		inline T operator+(T value) const {
			return ox::bigEndianAdapt(m_value) + value;
		}

		inline T operator+=(T other) {
			auto newVal = *this + other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		inline T operator-(T value) const {
			return ox::bigEndianAdapt(m_value) - value;
		}

		inline T operator-=(T other) {
			auto newVal = *this - other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		inline T operator*(T value) const {
			return ox::bigEndianAdapt(m_value) * value;
		}

		inline T operator*=(T other) {
			auto newVal = *this * other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		inline T operator/(T value) const {
			return ox::bigEndianAdapt(m_value) / value;
		}

		inline T operator/=(T other) {
			auto newVal = *this / other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		// Prefix increment
		inline T &operator++() {
			return operator+=(1);
		}

		// Postfix increment
		inline T operator++(int) {
			auto old = *this;
			++*this;
			return old;
		}

		// Prefix decrement
		inline T &operator--() {
			return operator-=(1);
		}

		// Postfix decrement
		inline T operator--(int) {
			auto old = *this;
			--*this;
			return old;
		}

		inline T operator~() const {
			return ~ox::bigEndianAdapt(m_value);
		}

		inline T operator&(T value) const {
			return ox::bigEndianAdapt(m_value) & value;
		}

		inline T operator&=(T other) {
			auto newVal = *this & other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		inline T operator|(T value) const {
			return ox::bigEndianAdapt(m_value) | value;
		}

		inline T operator|=(T other) {
			auto newVal = *this | other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		inline T operator^(T value) const {
			return ox::bigEndianAdapt(m_value) ^ value;
		}

		inline T operator^=(T other) {
			auto newVal = *this ^ other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		inline T operator>>(T value) const {
			return ox::bigEndianAdapt(m_value) >> value;
		}

		inline T operator>>=(T other) {
			auto newVal = *this >> other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

		inline T operator<<(T value) const {
			return ox::bigEndianAdapt(m_value) << value;
		}

		inline T operator<<=(T other) {
			auto newVal = *this << other;
			m_value = ox::bigEndianAdapt(newVal);
			return newVal;
		}

};

}
