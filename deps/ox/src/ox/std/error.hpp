/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "strongint.hpp"
#include "typetraits.hpp"
#include "utility.hpp"

#ifdef DEBUG
#define OxError(x) ox::_error(__FILE__, __LINE__, static_cast<ox::Error>(x))
#else
#define OxError(x) static_cast<ox::Error>(x)
#endif

#define oxReturnError(x) if (const auto _err = ox::error::toError(x)) return _err

namespace ox {

struct BaseError {
	const char *file = "";
	uint16_t line = 0;

	BaseError() = default;

	constexpr BaseError(const BaseError &o) noexcept {
		file = o.file;
		line = o.line;
	}

};

using Error = Integer<uint64_t, BaseError>;

struct ErrorInfo {
	const char *file = nullptr;
	int line = -1;
	Error errCode = Error(0);

	ErrorInfo() = default;

	ErrorInfo(Error err) {
		this->file = err.file;
		this->line = err.line;
		this->errCode = err;
	}
};

static constexpr Error _error(const char *file, uint32_t line, Error errCode) {
	Error err = errCode;
	err.file = file;
	err.line = line;
	return err;
}

template<typename T>
struct ValErr {
	T value;
	Error error;

	constexpr ValErr() noexcept: error(0) {
	}

	constexpr ValErr(Error error) noexcept: value(ox::move(value)), error(error) {
		this->error = error;
	}

	constexpr ValErr(T value, Error error = OxError(0)) noexcept: value(ox::move(value)), error(error) {
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

	[[nodiscard]] constexpr ox::Error get(T *val) noexcept {
		*val = value;
		return error;
	}

};

namespace error {

[[nodiscard]] constexpr ox::Error toError(ox::Error e) noexcept {
	return e;
}

template<typename T>
[[nodiscard]] constexpr ox::Error toError(ox::ValErr<T> ve) noexcept {
	return ve.error;
}

}

}

