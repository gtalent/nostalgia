/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace std {

using nullptr_t = decltype(nullptr);
using size_t = decltype(alignof(int));

}

#if __has_include(<cstdint>)

#include <cstdint>

#else

typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef short              int16_t;
typedef unsigned short     uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
typedef unsigned           uint_t;
#if defined(__arm__) || defined(__ppc__)
typedef long long          int64_t;
typedef unsigned long long uint64_t;
typedef __INTMAX_TYPE__    intmax_t;
typedef __UINTMAX_TYPE__   uintmax_t;
#else
typedef long               int64_t;
typedef unsigned long      uint64_t;
typedef int64_t            intmax_t;
typedef uint64_t           uintmax_t;
#endif

#if defined(_LP64) || defined(__ppc64__) || defined(__aarch64__)
typedef long          intptr_t;
typedef unsigned long uintptr_t;
#elif defined(_WIN64)
typedef int64_t  intptr_t;
typedef uint64_t uintptr_t;
#elif defined(_LP32) || defined(__ppc__) || defined(_WIN32) || defined(__arm__)
typedef int32_t  intptr_t;
typedef uint32_t uintptr_t;
#else
#error intptr_t, and uintptr_t undefined
#endif

#endif

namespace ox {

/**
 * Aliases type T in size and alignment to allow allocating space for a T
 * without running the constructor.
 */
template<typename T, std::size_t sz = sizeof(T)>
struct alignas(alignof(T)) AllocAlias {
	char buff[sz];
};


template<std::size_t sz>
struct SignedType {
};

template<>
struct SignedType<8> {
	using type = int8_t;
};

template<>
struct SignedType<16> {
	using type = int16_t;
};

template<>
struct SignedType<32> {
	using type = int32_t;
};

template<>
struct SignedType<64> {
	using type = int64_t;
};


template<std::size_t sz>
struct UnsignedType {
};

template<>
struct UnsignedType<8> {
	using type = uint8_t;
};

template<>
struct UnsignedType<16> {
	using type = uint16_t;
};

template<>
struct UnsignedType<32> {
	using type = uint32_t;
};

template<>
struct UnsignedType<64> {
	using type = uint64_t;
};

template<std::size_t bits>
using Int = typename SignedType<bits>::type;

template<std::size_t bits>
using Uint = typename UnsignedType<bits>::type;

template<typename T>
using Signed = Int<sizeof(T) * 8>;

template<typename T>
using Unsigned = Uint<sizeof(T) * 8>;

// ResizedInt retains the sign while granting a new size
template<typename T,  std::size_t bits, bool si = T(-1) < T(0)>
struct ResizedInt {
};

template<typename T,  std::size_t bits>
struct ResizedInt<T, bits, true> {
	using type = typename SignedType<bits>::type;
};

template<typename T,  std::size_t bits>
struct ResizedInt<T, bits, false> {
	using type = typename UnsignedType<bits>::type;
};

template<typename T,  std::size_t bits>
using ResizedInt_t = typename ResizedInt<T, bits>::type;

}


static_assert(sizeof(int8_t)   == 1, "int8_t is wrong size");
static_assert(sizeof(int16_t)  == 2, "int16_t is wrong size");
static_assert(sizeof(int32_t)  == 4, "int32_t is wrong size");
static_assert(sizeof(int64_t)  == 8, "int64_t is wrong size");
static_assert(sizeof(intptr_t) == sizeof(void*), "intptr_t is wrong size");

static_assert(sizeof(uint8_t)   == 1, "uint8_t is wrong size");
static_assert(sizeof(uint16_t)  == 2, "uint16_t is wrong size");
static_assert(sizeof(uint32_t)  == 4, "uint32_t is wrong size");
static_assert(sizeof(uint64_t)  == 8, "uint64_t is wrong size");
static_assert(sizeof(uintptr_t) == sizeof(void*), "uintptr_t is wrong size");
