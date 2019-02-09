/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/__buildinfo/defines.hpp>

#include "error.hpp"

namespace ox {

template<typename T>
void _assert(const char*, int, T, const char*) {
}

template<>
void _assert<bool>(const char *file, int line, bool pass, const char *msg);

template<>
void _assert<Error>(const char *file, int line, Error err, const char*);

}

#ifndef NDEBUG
#define oxAssert(pass, msg) ox::_assert<decltype(pass)>(__FILE__, __LINE__, pass, msg)
#else
#define oxAssert(pass, msg)
#endif
