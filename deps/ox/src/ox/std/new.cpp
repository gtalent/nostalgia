/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef OX_USE_STDLIB

#include "types.hpp"

void *operator new(std::size_t, void *addr) noexcept {
	return addr;
}

void *operator new[](std::size_t, void *addr) noexcept {
	return addr;
}

#endif
