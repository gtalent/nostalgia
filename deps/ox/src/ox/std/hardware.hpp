/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#if defined(__x86_64__) || defined(_M_AMD64)
#define OX_ARCH_x86_64
#elif defined(__i386__) || defined(_M_IX86)
#define OX_ARCH_x86_32
#elif defined(_M_AMD64)
#define OX_ARCH_ARM64
#elif defined(__arm__)
#define OX_ARCH_ARM
#endif

#if defined(OX_ARCH_ARM)

#if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__)
#define OX_HW_DIV 1
#else
#define OX_HW_DIV 0
#endif

#elif defined(OX_ARCH_x86_32) || defined(OX_ARCH_x86_64) || defined(OX_ARCH_ARM64)

#define OX_HW_DIV 1

#else

#error "Undefined hardware"

#endif

