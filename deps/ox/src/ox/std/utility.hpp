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
constexpr typename remove_reference<T>::type &&move(T &&t) noexcept {
	return static_cast<typename remove_reference<T>::type&&>(t);
}

}
