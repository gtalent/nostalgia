/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace ox::defines {

#if defined(OX_NODEBUG)
constexpr bool NoDebug = true;
#else
constexpr bool NoDebug = false;
#endif

#if defined(OX_USE_STDLIB)
constexpr auto UseStdLib = true;
#else
constexpr auto UseStdLib = false;
#endif

#if defined(DEBUG)
constexpr auto Debug = true;
#else
constexpr auto Debug = false;
#endif

#if defined(NDEBUG)
constexpr auto NDebug = true;
#else
constexpr auto NDebug = false;
#endif

#if defined(__BIG_ENDIAN__)
constexpr auto BigEndian = true;
constexpr auto LittleEndian = false;
#else
constexpr auto BigEndian = false;
constexpr auto LittleEndian = true;
#endif

enum class OS {
	BareMetal,
	NetBSD,
	OpenBSD,
	FreeBSD,
	DragonFlyBSD,
	Linux,
	Darwin,
	Windows
};

#if defined(__FreeBSD__)
constexpr OS OS = OS::FreeBSD;
#define OX_OS_FreeBSD
#elif defined(__NetBSD__)
constexpr OS OS = OS::NetBSD;
#define OX_OS_NetBSD
#elif defined(__OpenBSD__)
constexpr OS OS = OS::OpenBSD;
#define OX_OS_OpenBSD
#elif defined(__DragonFly__)
constexpr OS OS = OS::DragonFlyBSD;
#define OX_OS_DragonFlyBSD
#elif defined(__gnu_linux__)
constexpr OS OS = OS::Linux;
#define OX_OS_Linux
#elif defined(_WIN32)
constexpr OS OS = OS::Windows;
#define OX_OS_Windows
#elif defined(__APPLE__)
constexpr OS OS = OS::Darwin;
#define OX_OS_Darwin
#else
constexpr OS OS = OS::BareMetal;
#define OX_OS_BareMetal
#endif

}
