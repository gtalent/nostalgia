/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "typetraits.hpp"

namespace ox {

template<typename T>
[[nodiscard]]
constexpr const T &min(const T &a, const T &b) noexcept {
	return a < b ? a : b;
}

template<typename T>
[[nodiscard]]
constexpr const T &max(const T &a, const T &b) noexcept {
	return a > b ? a : b;
}

template<typename I>
[[nodiscard]]
constexpr I pow(I v, int e) noexcept {
	I out = 1;
	for (I i = 0; i < e; i++) {
		out *= v;
	}
	return out;
}

}
