/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"

#if defined(_MSC_VER)
#include <malloc.h>
#define ox_alloca(size) _alloca(size)
#elif !defined(OX_USE_STDLIB)
#define ox_alloca(size) __builtin_alloca(size)
#else
#include <cstdlib>
#include <alloca.h>
#define ox_alloca(size) alloca(size)
#endif


namespace ox {

constexpr auto MaxAllocaSize = 10 * 1024;

}

#if defined(OX_USE_STDLIB)
#define ox_malloca(size) size > MaxAllocaSize ? malloc(MaxAllocaSize) : ox_alloca(size)
#else
#define ox_malloca(size) ox_alloca(size)
#endif

inline void ox_freea(std::size_t size, void *ptr) {
	if constexpr(ox::defines::UseStdLib) {
		if (size > ox::MaxAllocaSize) {
			free(ptr);
		}
	}
}


void* operator new(std::size_t, void*) noexcept;
