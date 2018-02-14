/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/std.hpp>

#include "trace.hpp"

namespace ox {

OutStream::OutStream(const char *file, int line, const char *ch, const char *msg) {
	m_msg.file = file;
	m_msg.line = line;
	m_msg.ch = ch;
	m_msg.msg = msg;
}

}
