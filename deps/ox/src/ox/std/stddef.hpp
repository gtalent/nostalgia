/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#if OX_USE_STDLIB
#include <cstddef>
#else
#define offsetof(type, member) __builtin_offsetof(type, member)
#endif

#ifdef _MSC_VER
#define OX_PACKED
#else
#define OX_PACKED __attribute__((packed))
#endif
