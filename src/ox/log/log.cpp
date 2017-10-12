/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdarg.h>
#include <stdio.h>

#include "log.hpp"

namespace ox {

class Logger {

	private:
		// void* stand-in for FILE*
		const char *m_path = nullptr;

	public:
		Logger(const char *path = nullptr);

		~Logger();

		void log(LogLevel_t level, const char *msg, va_list args);

		void info(const char *msg, ...);

		void debug(const char *msg, ...);

		void error(const char *msg, ...);
};

Logger::Logger(const char *path) {
	m_path = path;
}

Logger::~Logger() {
}

void Logger::log(LogLevel_t level, const char *msg, va_list args) {
	if (m_path) {
		auto file = fopen(m_path, "a");
		vfprintf(file, msg, args);
		fprintf(file, "\n");
		fclose(file);
	}
}

void Logger::info(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	log(LogLevel_t::Info, msg, args);
	va_end(args);
}

void Logger::debug(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	log(LogLevel_t::Debug, msg, args);
	va_end(args);
}

void Logger::error(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	log(LogLevel_t::Error, msg, args);
	va_end(args);
}


static Logger logger;

void logFile(const char *path) {
	logger = Logger(path);
}

void log(LogLevel_t level, const char *msg, va_list args) {
	logger.log(level, msg, args);
}

void info(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	log(LogLevel_t::Info, msg, args);
	va_end(args);
}

void debug(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	log(LogLevel_t::Debug, msg, args);
	va_end(args);
}

void error(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	log(LogLevel_t::Error, msg, args);
	va_end(args);
}

}
