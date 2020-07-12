/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#ifdef OX_USE_STDLIB
#include <cstddef>
#else
#define offsetof(type, member) __builtin_offsetof(type, member)
#endif

#ifdef _MSC_VER
#define OX_PACKED
#else
#define OX_PACKED __attribute__((packed))
#endif

#ifdef _MSC_VER
#define OX_ALIGN4
#else
#define OX_ALIGN4 __attribute__((aligned(4)))
#endif

#ifdef _MSC_VER
#define OX_ALIGN8
#else
#define OX_ALIGN8 __attribute__((aligned(8)))
#endif
