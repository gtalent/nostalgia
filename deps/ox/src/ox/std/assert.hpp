/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "defines.hpp"
#include "error.hpp"

namespace ox {

template<typename T>
void assertFunc(const char*, int, T, const char*) {
}

template<>
void assertFunc<bool>(const char *file, int line, bool pass, const char *msg);

template<>
void assertFunc<Error>(const char *file, int line, Error err, const char*);

void panic([[maybe_unused]]const char *file, [[maybe_unused]]int line, [[maybe_unused]]const char *msg, [[maybe_unused]]Error err = OxError(0));

}

#define oxPanic(errCode, msg) ox::panic(__FILE__, __LINE__, msg, errCode)
#ifndef NDEBUG
#define oxAssert(pass, msg) ox::assertFunc<decltype(pass)>(__FILE__, __LINE__, pass, msg)
#else
inline void oxAssert(bool, const char*) {}
inline void oxAssert(ox::Error, const char*) {}
#endif
