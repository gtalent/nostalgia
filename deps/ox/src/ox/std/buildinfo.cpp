/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

namespace ox::buildinfo {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"

#if defined(OX_USE_STDLIB)
const bool UseStdLib = true;
#else
const bool UseStdLib = false;
#endif

#if defined(DEBUG)
const bool Debug = true;
#else
const bool Debug = false;
#endif

#if defined(NDEBUG)
const bool NDebug = true;
#else
const bool NDebug = false;
#endif

#if defined(__BIG_ENDIAN__)
const bool BigEndian = true;
const bool LittleEndian = false;
#else
const bool BigEndian = false;
const bool LittleEndian = true;
#endif

#pragma clang diagnostic pop

}
