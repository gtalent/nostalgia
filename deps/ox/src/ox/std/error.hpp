/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "defines.hpp"
#include "strongint.hpp"
#include "typetraits.hpp"
#include "utility.hpp"

#define OxError(...) ox::_error(__FILE__, __LINE__, __VA_ARGS__)

namespace ox {

struct [[nodiscard]] Error {
	const char *msg = nullptr;
	const char *file = "";
	uint16_t line = 0;
	uint64_t m_i = 0;

	explicit constexpr Error(uint64_t i = 0) {
		m_i = i;
	}

	constexpr Error(const Error &o) noexcept {
		msg = o.msg;
		file = o.file;
		line = o.line;
		m_i = o.m_i;
	}

	constexpr Error &operator=(const Error &o) noexcept {
		msg = o.msg;
		file = o.file;
		line = o.line;
		m_i = o.m_i;
		return *this;
	}

	constexpr operator uint64_t() const noexcept {
		return m_i;
	}

};

constexpr Error _error(const char *file, uint32_t line, uint64_t errCode, const char *msg = nullptr) {
	auto err = static_cast<Error>(errCode);
	err.file = file;
	err.line = line;
	err.msg = msg;
	return err;
}

template<typename T>
struct [[nodiscard]] Result {
	T value;
	Error error;

	constexpr Result() noexcept: error(0) {
	}

	constexpr Result(Error error) noexcept: value(ox::move(value)), error(error) {
		this->error = error;
	}

	constexpr Result(T value, Error error = OxError(0)) noexcept: value(ox::move(value)), error(error) {
	}

	explicit constexpr operator const T&() const noexcept {
		return value;
	}

	explicit constexpr operator T&() noexcept {
		return value;
	}

	[[nodiscard]] constexpr bool ok() const noexcept {
		return error == 0;
	}

	constexpr Error get(T *val) noexcept {
		*val = value;
		return error;
	}

};

namespace error {

constexpr Error toError(Error e) noexcept {
	return e;
}

template<typename T>
constexpr Error toError(const Result<T> &ve) noexcept {
	return ve.error;
}

}

}

inline void oxIgnoreError(ox::Error) {}
#define oxReturnError(x) if (const auto _ox_error = ox::error::toError(x)) return _ox_error
#define oxThrowError(x) if (const auto _ox_error = ox::error::toError(x)) throw _ox_error

