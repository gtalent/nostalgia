/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>

namespace ox::trace {

struct TraceMsg {
	ox::BString<150> file = "";
	int line = 0;
	uint64_t time = 0;
	ox::BString<50> ch = "";
	ox::BString<100> msg;
};

template<typename T>
int ioOp(T *io, ox::trace::TraceMsg *obj) {
	int32_t err = 0;
	io->setFields(5);
	err |= io->op("file", &obj->file);
	err |= io->op("line", &obj->line);
	err |= io->op("time", &obj->time);
	err |= io->op("msg", &obj->msg);
	return err;
}

class OutStream {

	private:
		TraceMsg m_msg;

	public:
		OutStream() = default;

		OutStream(const char *file, int line, const char *ch, const char *msg = "");

		~OutStream();

		template<typename T>
		inline OutStream &operator<<(const T &v) {
			m_msg.msg += " ";
			m_msg.msg += v;
			return *this;
		}

};


class StdOutStream {

	private:
		TraceMsg m_msg;

	public:
		StdOutStream() = default;

		StdOutStream(const char *file, int line, const char *ch, const char *msg = "");

		~StdOutStream();

		template<typename T>
		inline StdOutStream &operator<<(T v) {
			m_msg.msg += " ";
			m_msg.msg += v;
			return *this;
		}

};


class NullStream {

	public:
		NullStream() = default;

		NullStream(const char *file, int line, const char *ch, const char *msg = "");

		~NullStream();

		template<typename T>
		inline NullStream &operator<<(const T&) {
			return *this;
		}

};

}

#define oxTrace(ch) ox::trace::NullStream(__FILE__, __LINE__, ch)
