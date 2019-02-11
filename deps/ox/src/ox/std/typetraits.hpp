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

template<class T, T v>
struct integral_constant {

	using value_type = T;
	using type = integral_constant;

	static constexpr T value = v;

	constexpr operator value_type() const noexcept {
		return value;
	}

	constexpr value_type operator()() const noexcept {
		return value;
	}

};

using false_type = ox::integral_constant<bool, false>;
using true_type = ox::integral_constant<bool, true>;


// is_integral /////////////////////////////////////////////////////////////////

template<typename T> struct is_integral: ox::false_type {};
template<> struct is_integral<bool>    : ox::true_type {};
template<> struct is_integral<wchar_t> : ox::true_type {};
template<> struct is_integral<int8_t>  : ox::true_type {};
template<> struct is_integral<uint8_t> : ox::true_type {};
template<> struct is_integral<int16_t> : ox::true_type {};
template<> struct is_integral<uint16_t>: ox::true_type {};
template<> struct is_integral<int32_t> : ox::true_type {};
template<> struct is_integral<uint32_t>: ox::true_type {};
template<> struct is_integral<int64_t> : ox::true_type {};
template<> struct is_integral<uint64_t>: ox::true_type {};


// enable_if ///////////////////////////////////////////////////////////////////

template<bool B, class T = void>
struct enable_if {
};

template<class T>
struct enable_if<true, T> {
	using type = T;
};

template<typename T>
struct RemoveIndirection {
		
	private:
		template<typename ST>
		static constexpr ST decay(ST t) {
			return t;
		}

		template<typename ST>
		static constexpr ST decay(ST *t) {
			return decay(*t);
		}

	public:
		using type = decltype(decay(static_cast<T*>(nullptr)));

};

}
