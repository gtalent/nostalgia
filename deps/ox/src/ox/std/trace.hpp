/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#ifdef OX_USE_STDLIB
#include <array>
#endif

#include "bstring.hpp"
#include "fmt.hpp"
#include "hashmap.hpp"
#include "units.hpp"

extern "C" {

void oxTraceInitHook();

void oxTraceHook(const char *file, int line, const char *ch, const char *msg);

}

namespace ox::trace {

struct TraceMsg {
	static constexpr auto TypeName = "net.drinkingtea.ox.trace.TraceMsg";
	static constexpr auto Fields = 5;
	static constexpr auto TypeVersion = 1;
	const char *file = "";
	int line = 0;
	uint64_t time = 0;
	const char *ch = "";
	BString<units::KB * 10> msg;
};

template<typename T>
constexpr Error model(T *io, TraceMsg *obj) {
	io->template setTypeInfo<TraceMsg>();
	oxReturnError(io->field("ch", &obj->ch));
	oxReturnError(io->field("file", &obj->file));
	oxReturnError(io->field("line", &obj->line));
	oxReturnError(io->field("time", &obj->time));
	oxReturnError(io->field("msg", &obj->msg));
	return OxError(0);
}

class OutStream {

	protected:
		const char *m_delimiter = " ";
		TraceMsg m_msg;

	public:
		constexpr OutStream(const char *file, int line, const char *ch, const char *msg = "") noexcept {
			m_msg.file = file;
			m_msg.line = line;
			m_msg.ch = ch;
			m_msg.msg = msg;
		}

#ifdef OX_USE_STDLIB
		template<std::size_t fmtSegmentCnt, typename ...Args>
		constexpr OutStream(const char *file, int line, const char *ch, detail::Fmt<fmtSegmentCnt> fmtSegments, std::array<detail::FmtArg, fmtSegmentCnt - 1> elements) noexcept {
			//static_assert(sizeof...(args) == fmtSegmentCnt - 1, "Wrong number of trace arguments for format.");
			m_msg.file = file;
			m_msg.line = line;
			m_msg.ch = ch;
			const auto &firstSegment = fmtSegments.segments[0];
			m_msg.msg.append(firstSegment.str, firstSegment.length);
			//const detail::FmtArg elements[sizeof...(args)] = {args...};
			for (auto i = 0u; i < fmtSegments.size - 1; ++i) {
				m_msg.msg += elements[i].out;
				const auto &s = fmtSegments.segments[i + 1];
				m_msg.msg.append(s.str, s.length);
			}
		}
#else
		template<std::size_t fmtSegmentCnt, typename ...Args>
		constexpr OutStream(const char *file, int line, const char *ch, detail::Fmt<fmtSegmentCnt> fmtSegments, Args... args) {
			//static_assert(sizeof...(args) == fmtSegmentCnt - 1, "Wrong number of trace arguments for format.");
			m_msg.file = file;
			m_msg.line = line;
			m_msg.ch = ch;
			const auto &firstSegment = fmtSegments.segments[0];
			m_msg.msg.append(firstSegment.str, firstSegment.length);
			const detail::FmtArg elements[sizeof...(args)] = {args...};
			for (auto i = 0u; i < fmtSegments.size - 1; ++i) {
				m_msg.msg += elements[i].out;
				const auto &s = fmtSegments.segments[i + 1];
				m_msg.msg.append(s.str, s.length);
			}
		}
#endif

		inline ~OutStream() noexcept {
			oxTraceHook(m_msg.file, m_msg.line, m_msg.ch, m_msg.msg.c_str());
		}

		template<typename T>
		constexpr OutStream &operator<<(const T &v) noexcept {
			if (m_msg.msg.len()) {
				m_msg.msg += m_delimiter;
			}
			m_msg.msg += v;
			return *this;
		}

		constexpr OutStream &operator<<(const Error &err) noexcept {
			if (m_msg.msg.len()) {
				m_msg.msg += m_delimiter;
			}
			m_msg.msg += static_cast<int64_t>(err);
			return *this;
		}

		/**
		 * del sets the delimiter between log segments.
		 */
		constexpr OutStream &del(const char *delimiter) noexcept {
			m_delimiter = delimiter;
			return *this;
		}

};


class NullStream {

	public:
		constexpr NullStream(const char*, int, const char*, const char* = "") noexcept {
		}

#ifdef OX_USE_STDLIB
		template<std::size_t fmtSegmentCnt, typename ...Args>
		constexpr NullStream(const char*, int, const char*, detail::Fmt<fmtSegmentCnt>, std::array<detail::FmtArg, fmtSegmentCnt - 1>) noexcept {
		}
#else
		template<std::size_t fmtSegmentCnt, typename ...Args>
		constexpr NullStream(const char*, int, const char*, detail::Fmt<fmtSegmentCnt>, Args...) {
		}
#endif

		template<typename T>
		constexpr NullStream &operator<<(const T&) noexcept {
			return *this;
		}

		constexpr NullStream &del(const char*) noexcept {
			return *this;
		}

};

#ifdef DEBUG
using TraceStream = OutStream;
#else
using TraceStream = NullStream;
#endif

void logError(const char *file, int line, const Error &err);

void init();

}

#define oxLogError(err) ox::trace::logError(__FILE__, __LINE__, err)

#define oxTrace(...) ox::trace::TraceStream(__FILE__, __LINE__, __VA_ARGS__)
#define oxOut(...) ox::trace::OutStream(__FILE__, __LINE__, "stdout", __VA_ARGS__)
#define oxErr(...) ox::trace::OutStream(__FILE__, __LINE__, "stderr", __VA_ARGS__)

#ifdef OX_USE_STDLIB
// Non-GCC compilers don't like ##__VA_ARGS__, so use initializer list, which relies on std lib
#define oxTracef(ch, fmt, ...) ox::trace::TraceStream(__FILE__, __LINE__, ch, ox::detail::fmtSegments<ox::detail::argCount(fmt)+1>(fmt), {__VA_ARGS__})
#define oxOutf(fmt, ...) ox::trace::OutStream(__FILE__, __LINE__, "stdout", ox::detail::fmtSegments<ox::detail::argCount(fmt)+1>(fmt), {__VA_ARGS__})
#define oxErrf(fmt, ...) ox::trace::OutStream(__FILE__, __LINE__, "stderr", ox::detail::fmtSegments<ox::detail::argCount(fmt)+1>(fmt), {__VA_ARGS__})
#else
#define oxTracef(ch, fmt, ...) ox::trace::TraceStream(__FILE__, __LINE__, ch, ox::detail::fmtSegments<ox::detail::argCount(fmt)+1>(fmt), ##__VA_ARGS__)
#define oxOutf(fmt, ...) ox::trace::OutStream(__FILE__, __LINE__, "stdout", ox::detail::fmtSegments<ox::detail::argCount(fmt)+1>(fmt), ##__VA_ARGS__)
#define oxErrf(fmt, ...) ox::trace::OutStream(__FILE__, __LINE__, "stderr", ox::detail::fmtSegments<ox::detail::argCount(fmt)+1>(fmt), ##__VA_ARGS__)
#endif

#define oxInfo(...) oxTrace("info", __VA_ARGS__)
#define oxInfof(...) oxTracef("info", __VA_ARGS__)
#define oxError(...) oxTrace("error", __VA_ARGS__)
#define oxErrorf(...) oxTracef("error", __VA_ARGS__)

#ifndef OX_NODEBUG
#define oxDebug(...) oxTrace("debug", __VA_ARGS__)
#define oxDebugf(...) oxTracef("debug", __VA_ARGS__)
#else
#define oxDebug(...) static_assert(false, "Debug prints were checked in."); oxTrace("debug", __VA_ARGS__)
#define oxDebugf(...) static_assert(false, "Debug prints were checked in."); oxTracef("debug", __VA_ARGS__)
#endif
