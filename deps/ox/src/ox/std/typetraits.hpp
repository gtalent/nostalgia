/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"

#if __has_include(<type_traits>)

#include <type_traits>

#else

namespace std {

template<typename T>
constexpr bool is_union_v = __is_union(T);

}

#endif

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

using false_type = integral_constant<bool, false>;
using true_type = integral_constant<bool, true>;


// is_integral /////////////////////////////////////////////////////////////////

template<typename T> struct is_integral: false_type {};
template<> struct is_integral<bool>    : true_type {};
template<> struct is_integral<wchar_t> : true_type {};
template<> struct is_integral<int8_t>  : true_type {};
template<> struct is_integral<uint8_t> : true_type {};
template<> struct is_integral<int16_t> : true_type {};
template<> struct is_integral<uint16_t>: true_type {};
template<> struct is_integral<int32_t> : true_type {};
template<> struct is_integral<uint32_t>: true_type {};

// some of these need to be done with the actual language syntax because no one
// can agree on what an (u)int64_t is...
template<> struct is_integral<long>: true_type {};
template<> struct is_integral<long long>: true_type {};
template<> struct is_integral<unsigned long>: true_type {};
template<> struct is_integral<unsigned long long>: true_type {};

template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template<typename T> struct is_char: false_type {};
template<> struct is_char<char>    : true_type {};
template<typename T>
constexpr bool is_char_v = is_char<T>::value;

template<typename T> struct is_bool: false_type {};
template<> struct is_bool<bool>    : true_type {};
template<typename T>
constexpr bool is_bool_v = is_bool<T>::value;

template<typename T> struct is_union: integral_constant<bool, std::is_union_v<T>> {};

template<typename T>
constexpr bool is_union_v = is_union<T>();

// indicates the type can have members, but not that it necessarily does
template<typename T>
constexpr bool memberable(int T::*) { return true; }
template<typename T>
constexpr bool memberable(...) { return false; }

template<typename T>
struct is_class: integral_constant<bool, !is_union<T>::value && memberable<T>(0)> {};

namespace test {
struct TestClass {int i;};
union TestUnion {int i;};
static_assert(is_class<TestClass>::value == true);
static_assert(is_class<TestUnion>::value == false);
static_assert(is_class<int>::value == false);
}

template<typename T>
constexpr bool is_class_v = is_class<T>();

template<typename T>
constexpr bool is_signed_v = integral_constant<bool, T(-1) < T(0)>::value;

// enable_if ///////////////////////////////////////////////////////////////////

template<bool B, class T = void>
struct enable_if {
};

template<class T>
struct enable_if<true, T> {
	using type = T;
};


template<typename T>
struct is_pointer {
	static constexpr bool value = false;
};

template<typename T>
struct is_pointer<T*> {
	static constexpr bool value = true;
};

template<typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;

template<typename T>
struct remove_pointer {
	using type = T;
};

template<typename T>
struct remove_pointer<T*> {
	using type = T;
};

template<typename T>
struct remove_pointer<T* const> {
	using type = T;
};

template<typename T>
struct remove_pointer<T* volatile> {
	using type = T;
};

template<typename T>
struct remove_pointer<T* const volatile> {
	using type = T;
};


template<typename T>
struct remove_reference {
	using type = T;
};

template<typename T>
struct remove_reference<T&> {
	using type = T;
};

template<typename T>
struct remove_reference<T&&> {
	using type = T;
};

}
