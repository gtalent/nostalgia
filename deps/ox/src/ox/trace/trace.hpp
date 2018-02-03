/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace ox {

void trace(const char *file, int line, const char *ch, const char *msg);

}

#define ox_trace(ch, msg) ox::trace(__FILE__, __LINE__, ch, msg)
