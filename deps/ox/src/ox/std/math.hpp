/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "typetraits.hpp"

namespace ox {

template<typename T>
inline constexpr const T &min(const T &a, const T &b) {
	return a < b ? a : b;
}

template<typename T>
inline constexpr const T &max(const T &a, const T &b) {
	return a > b ? a : b;
}

template<typename I>
inline constexpr I pow(I v, int e) {
	I out = 1;
	for (I i = 0; i < e; i++) {
		out *= v;
	}
	return out;
}

}
