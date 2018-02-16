/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/mc/write.hpp>

#include "trace.hpp"

namespace ox {

OutStream::OutStream(const char *file, int line, const char *ch, const char *msg) {
	m_msg.file = file;
	m_msg.line = line;
	m_msg.ch = ch;
	m_msg.msg = msg;
}

OutStream::~OutStream() {
	constexpr size_t buffLen = 1024;
	size_t size = 0;
	uint8_t buff[buffLen];
	writeMC(buff, buffLen, &m_msg, &size);
}

}
