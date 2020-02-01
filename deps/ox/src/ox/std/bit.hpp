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

template<typename T>
[[nodiscard]] constexpr T rotl(T i, int shift) noexcept {
	constexpr auto bits = sizeof(i) * 8;
	return (i << shift) | (i >> (bits - shift));
}

template<typename T>
[[nodiscard]] constexpr T onMask(int bits = sizeof(T) << 3 /* *8 */) noexcept {
	T out = T(0);
	for (auto i = 0; i < bits; i++) {
		out |= static_cast<T>(1) << i;
	}
	return out;
}

static_assert(onMask<int>(1) == 1);
static_assert(onMask<int>(2) == 3);
static_assert(onMask<int>(3) == 7);
static_assert(onMask<int>(4) == 15);

}
