/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if defined(OX_USE_STDLIB)
#include <iomanip>
#include <iostream>
#include <stdio.h>
#endif

#include "trace.hpp"

extern "C"
void oxTraceHook([[maybe_unused]] const char *file, [[maybe_unused]] int line,
                 [[maybe_unused]] const char *ch, [[maybe_unused]] const char *msg) {
}

namespace ox::trace {

#if defined(OX_USE_STDLIB)
static const auto OxPrintTrace = std::getenv("OXTRACE");
#else
constexpr auto OxPrintTrace = false;
#endif

OutStream::OutStream(const char *file, int line, const char *ch, const char *msg) {
	m_msg.file = file;
	m_msg.line = line;
	m_msg.ch = ch;
	m_msg.msg = msg;
}

OutStream::~OutStream() {
	oxTraceHook(m_msg.file.c_str(), m_msg.line, m_msg.ch.c_str(), m_msg.msg.c_str());
}


StdOutStream::StdOutStream(const char *file, int line, const char *ch, const char *msg) {
	m_msg.file = file;
	m_msg.line = line;
	m_msg.ch = ch;
	m_msg.msg = msg;
}

StdOutStream::~StdOutStream() {
	oxTraceHook(m_msg.file.c_str(), m_msg.line, m_msg.ch.c_str(), m_msg.msg.c_str());
#if defined(OX_USE_STDLIB)
	if (OxPrintTrace) {
		std::cout << std::setw(53) << std::left << m_msg.ch.c_str() << '|';
		std::cout << std::setw(65) << std::left << m_msg.msg.c_str() << '|';
		std::cout << " " << m_msg.file.c_str() << ':' << m_msg.line << "\n";
	}
#endif
}


void logError(const char *file, int line, Error err) {
	if (err) {
		TraceStream trc(file, line, "ox::error");
		trc <<  "Error:" << err;
		if (err.file != nullptr) {
			trc << "(" << reinterpret_cast<const char*>(err.file) << ":" << err.line << ")";
		}
	}
}

}
