/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>

namespace ox {

struct TraceMsg {
	const char *file;
	int line;
	uint64_t time;
	const char *ch;
	ox::BString<100> msg;
};

class OutStream {

	private:
		TraceMsg m_msg;

	public:
		OutStream() = default;

		OutStream(const char *file, int line, const char *ch, const char *msg = "");

		template<typename T>
		OutStream &operator<<(T v) {
			m_msg.msg += " ";
			m_msg.msg += v;
			return *this;
		}

};

}

#define oxTrace(ch, msg) ox::OutStream(__FILE__, __LINE__, ch, msg)
