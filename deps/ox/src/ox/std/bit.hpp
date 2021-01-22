/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "memops.hpp"
#include "types.hpp"
#include "typetraits.hpp"

namespace ox {

template<typename To, typename From>
typename enable_if<sizeof(To) == sizeof(From), To>::type bit_cast(From src) noexcept {
	To dst;
	memcpy(&dst, &src, sizeof(src));
	return dst;
}

template<typename T>
[[nodiscard]] constexpr T rotl(T i, int shift) noexcept {
	constexpr auto bits = sizeof(i) * 8;
	return (i << static_cast<T>(shift)) | (i >> (bits - static_cast<T>(shift)));
}

template<typename T>
[[nodiscard]] constexpr T onMask(int bits = sizeof(T) << 3 /* *8 */) noexcept {
	T out = T(0);
	for (auto i = 0; i < bits; i++) {
		out |= static_cast<T>(1) << i;
	}
	return out;
}

template<typename T>
constexpr auto MaxValue = onMask<T>();

static_assert(onMask<int>(1) == 1);
static_assert(onMask<int>(2) == 3);
static_assert(onMask<int>(3) == 7);
static_assert(onMask<int>(4) == 15);

}
