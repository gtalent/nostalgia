/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace ox {

void _assert(const char *file, int line, bool pass, const char *msg);

}

#ifndef NDEBUG
#define oxAssert(pass, msg) ox::_assert(__FILE__, __LINE__, pass, msg)
#else
#define oxAssert(pass, msg)
#endif
