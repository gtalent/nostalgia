/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#if defined(__arm__)

#if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__)
#define OX_HW_DIV 1
#else
#define OX_HW_DIV 0
#endif

#elif defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)

#define OX_HW_DIV 1

#else

#error "Undefined hardware"

#endif

