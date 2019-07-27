/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "types.hpp"

namespace ox {

/**
 * Integer is a strongly typed integer wrapper used to create strongly typed
 * integers.
 */
template<typename T>
class Integer {
	private:
		T m_i;

	public:
		constexpr Integer() noexcept = default;

		constexpr explicit Integer(T i) noexcept;

		constexpr Integer<T> operator=(Integer<T> i) noexcept;

		constexpr Integer<T> operator==(Integer<T> i) const noexcept;

		constexpr Integer<T> operator!=(Integer<T> i) const noexcept;

		constexpr Integer<T> operator<(Integer<T> i) const noexcept;

		constexpr Integer<T> operator>(Integer<T> i) const noexcept;

		constexpr Integer<T> operator<=(Integer<T> i) const noexcept;

		constexpr Integer<T> operator>=(Integer<T> i) const noexcept;

		constexpr Integer<T> operator+(Integer<T> i) const noexcept;

		constexpr Integer<T> operator+=(Integer<T> i) noexcept;

		constexpr Integer<T> operator-(Integer<T> i) const noexcept;

		constexpr Integer<T> operator-=(Integer<T> i) noexcept;

		constexpr Integer<T> operator*(Integer<T> i) const noexcept;

		constexpr Integer<T> operator*=(Integer<T> i) noexcept;

		constexpr Integer<T> operator/(Integer<T> i) const noexcept;

		constexpr Integer<T> operator/=(Integer<T> i) noexcept;

		constexpr Integer<T> operator%(Integer<T> i) const noexcept;

		constexpr Integer<T> operator%=(Integer<T> i) noexcept;

		constexpr Integer<T> operator>>(Integer<T> i) const noexcept;

		constexpr Integer<T> operator>>=(Integer<T> i) noexcept;

		constexpr Integer<T> operator<<(Integer<T> i) const noexcept;

		constexpr Integer<T> operator<<=(Integer<T> i) noexcept;

		constexpr Integer<T> operator|(Integer<T> i) const noexcept;

		constexpr Integer<T> operator|=(Integer<T> i) noexcept;

		constexpr Integer<T> operator&(Integer<T> i) const noexcept;

		constexpr Integer<T> operator&=(Integer<T> i) noexcept;

		constexpr Integer<T> operator^(Integer<T> i) const noexcept;

		constexpr Integer<T> operator^=(Integer<T> i) noexcept;

		// Prefix increment
		constexpr Integer<T> operator++() noexcept;

		// Postfix increment
		constexpr Integer<T> operator++(int) noexcept;

		// Prefix decrement
		constexpr Integer<T> operator--() noexcept;

		// Postfix decrement
		constexpr Integer<T> operator--(int) noexcept;

		// Postfix decrement
		constexpr explicit operator T() const noexcept;

};

template<typename T>
constexpr Integer<T>::Integer(T i) noexcept {
	m_i = i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator=(Integer<T> i) noexcept {
	return m_i = i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator==(Integer<T> i) const noexcept {
	return m_i == i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator!=(Integer<T> i) const noexcept {
	return m_i != i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator<(Integer<T> i) const noexcept {
	return m_i < i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator>(Integer<T> i) const noexcept {
	return m_i > i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator<=(Integer<T> i) const noexcept {
	return m_i <= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator>=(Integer<T> i) const noexcept {
	return m_i >= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator+(Integer<T> i) const noexcept {
	return m_i + i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator+=(Integer<T> i) noexcept {
	return m_i += i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator-(Integer<T> i) const noexcept {
	return m_i - i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator-=(Integer<T> i) noexcept {
	return m_i -= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator*(Integer<T> i) const noexcept {
	return m_i * i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator*=(Integer<T> i) noexcept {
	return m_i *= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator/(Integer<T> i) const noexcept {
	return m_i / i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator/=(Integer<T> i) noexcept {
	return m_i /= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator%(Integer<T> i) const noexcept {
	return m_i % i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator%=(Integer<T> i) noexcept {
	return m_i %= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator|(Integer<T> i) const noexcept {
	return m_i | i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator|=(Integer<T> i) noexcept {
	return m_i |= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator&(Integer<T> i) const noexcept {
	return m_i & i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator&=(Integer<T> i) noexcept {
	return m_i &= i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator^(Integer<T> i) const noexcept {
	return m_i ^ i.m_i;
}

template<typename T>
constexpr Integer<T> Integer<T>::operator^=(Integer<T> i) noexcept {
	return m_i ^= i.m_i;
}

// Prefix increment
template<typename T>
constexpr inline Integer<T> Integer<T>::operator++() noexcept {
	return Integer<T>(++m_i);
}

// Postfix increment
template<typename T>
constexpr inline Integer<T> Integer<T>::operator++(int) noexcept {
	return Integer<T>(m_i++);
}

// Prefix decrement
template<typename T>
constexpr inline Integer<T> Integer<T>::operator--() noexcept {
	return Integer<T>(--m_i);
}

// Postfix decrement
template<typename T>
constexpr inline Integer<T> Integer<T>::operator--(int) noexcept {
	return Integer<T>(m_i--);
}

template<typename T>
constexpr Integer<T>::operator T() const noexcept {
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
