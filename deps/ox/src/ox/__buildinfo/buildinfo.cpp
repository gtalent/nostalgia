/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

namespace ox::buildinfo {

#if defined(OX_USE_STDLIB)
const auto UseStdLib = true;
#else
const auto UseStdLib = false;
#endif

#if defined(DEBUG)
const auto Debug = true;
#else
const auto Debug = false;
#endif

#if defined(NDEBUG)
const auto NDebug = true;
#else
const auto NDebug = false;
#endif

#if defined(__BIG_ENDIAN__)
const auto BigEndian = true;
const auto LittleEndian = false;
#else
const auto BigEndian = false;
const auto LittleEndian = true;
#endif

}
