/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"

namespace ox {

class BaseInteger {};

/**
 * Integer is a strongly typed integer wrapper used to create strongly typed
 * integers.
 */
template<typename T, class Base = BaseInteger>
class Integer: Base {
	private:
		T m_i;

	public:
		constexpr Integer() noexcept = default;

		constexpr explicit Integer(T i) noexcept;

		constexpr Integer<T, Base> operator=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator==(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator!=(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator<(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator>(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator<=(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator>=(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator+(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator+=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator-(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator-=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator*(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator*=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator/(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator/=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator%(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator%=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator>>(int i) const noexcept;

		constexpr Integer<T, Base> operator>>=(int i) noexcept;

		constexpr Integer<T, Base> operator<<(int i) const noexcept;

		constexpr Integer<T, Base> operator<<=(int i) noexcept;

		constexpr Integer<T, Base> operator|(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator|=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator&(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator&=(Integer<T, Base> i) noexcept;

		constexpr Integer<T, Base> operator^(Integer<T, Base> i) const noexcept;

		constexpr Integer<T, Base> operator^=(Integer<T, Base> i) noexcept;

		// Prefix increment
		constexpr Integer<T, Base> operator++() noexcept;

		// Postfix increment
		constexpr Integer<T, Base> operator++(int) noexcept;

		// Prefix decrement
		constexpr Integer<T, Base> operator--() noexcept;

		// Postfix decrement
		constexpr Integer<T, Base> operator--(int) noexcept;

		constexpr explicit operator T() const noexcept;

		constexpr operator bool() const noexcept;

};

template<typename T, class Base>
constexpr Integer<T, Base>::Integer(T i) noexcept {
	m_i = i;
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i = i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator==(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i == i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator!=(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i != i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator<(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i < i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator>(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i > i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator<=(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i <= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator>=(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i >= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator+(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i + i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator+=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i += i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator-(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i - i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator-=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i -= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator*(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i * i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator*=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i *= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator/(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i / i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator/=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i /= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator%(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i % i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator%=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i %= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator>>(int i) const noexcept {
	return Integer<T, Base>(m_i >> i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator>>=(int i) noexcept {
	return Integer<T, Base>(m_i >>= i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator<<(int i) const noexcept {
	return Integer<T, Base>(m_i << i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator<<=(int i) noexcept {
	return Integer<T, Base>(m_i <<= i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator|(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i | i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator|=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i |= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator&(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i & i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator&=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i &= i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator^(Integer<T, Base> i) const noexcept {
	return Integer<T, Base>(m_i ^ i.m_i);
}

template<typename T, class Base>
constexpr Integer<T, Base> Integer<T, Base>::operator^=(Integer<T, Base> i) noexcept {
	return Integer<T, Base>(m_i ^= i.m_i);
}

// Prefix increment
template<typename T, class Base>
constexpr inline Integer<T, Base> Integer<T, Base>::operator++() noexcept {
	return Integer<T, Base>(++m_i);
}

// Postfix increment
template<typename T, class Base>
constexpr inline Integer<T, Base> Integer<T, Base>::operator++(int) noexcept {
	return Integer<T, Base>(m_i++);
}

// Prefix decrement
template<typename T, class Base>
constexpr inline Integer<T, Base> Integer<T, Base>::operator--() noexcept {
	return Integer<T, Base>(--m_i);
}

// Postfix decrement
template<typename T, class Base>
constexpr inline Integer<T, Base> Integer<T, Base>::operator--(int) noexcept {
	return Integer<T, Base>(m_i--);
}

template<typename T, class Base>
constexpr Integer<T, Base>::operator T() const noexcept {
	return m_i;
}

template<typename T, class Base>
constexpr Integer<T, Base>::operator bool() const noexcept {
	return m_i;
}

using Int8  = Integer<int8_t>;
using Int16 = Integer<int16_t>;
using Int32 = Integer<int32_t>;
using Int64 = Integer<int64_t>;

using Uint8  = Integer<uint8_t>;
using Uint16 = Integer<uint16_t>;
using Uint32 = Integer<uint32_t>;
using Uint64 = Integer<uint64_t>;

}
