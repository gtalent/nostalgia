/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace ox {

template<typename T>
inline const T &min(const T &a, const T &b) {
	return a < b ? a : b;
}

template<typename T>
inline const T &max(const T &a, const T &b) {
	return a > b ? a : b;
}

}
