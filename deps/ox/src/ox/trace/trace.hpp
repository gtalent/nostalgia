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
	ox::BString<255> file = "";
	int line = 0;
	uint64_t time = 0;
	ox::BString<75> ch = "";
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
		const char *m_delimiter = " ";
		TraceMsg m_msg;

	public:
		OutStream() = default;

		OutStream(const char *file, int line, const char *ch, const char *msg = "");

		~OutStream();

		template<typename T>
		inline OutStream &operator<<(const T &v) {
			m_msg.msg += m_delimiter;
			m_msg.msg += v;
			return *this;
		}

		/**
		 * del sets the delimiter between log segments.
		 */
		inline OutStream &del(const char *delimiter) {
			m_delimiter = delimiter;
			return *this;
		}

};


class StdOutStream {

	private:
		const char *m_delimiter = " ";
		TraceMsg m_msg;

	public:
		StdOutStream() = default;

		StdOutStream(const char *file, int line, const char *ch, const char *msg = "");

		~StdOutStream();

		template<typename T>
		constexpr inline StdOutStream &operator<<(const T &v) {
			m_msg.msg += m_delimiter;
			m_msg.msg += v;
			return *this;
		}

		/**
		 * del sets the delimiter between log segments.
		 */
		inline StdOutStream &del(const char *delimiter) {
			m_delimiter = delimiter;
			return *this;
		}

};


class NullStream {

	public:
		constexpr NullStream() = default;

		constexpr NullStream(const char*, int, const char*, const char* = "") {
		}

		~NullStream() = default;

		template<typename T>
		constexpr inline NullStream &operator<<(const T&) {
			return *this;
		}

		inline NullStream &del(const char*) {
			return *this;
		}

};

#ifdef DEBUG
using TraceStream = OutStream;
#else
using TraceStream = NullStream;
#endif

void logError(const char *file, int line, Error err);

}

#define oxLogError(err) ox::trace::logError(__FILE__, __LINE__, err)

#define oxTrace(ch) ox::trace::TraceStream(__FILE__, __LINE__, ch)
