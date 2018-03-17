/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace ox::defines {

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

}
