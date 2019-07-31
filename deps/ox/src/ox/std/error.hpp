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

class BaseError {};

using Error = Integer<uint64_t, BaseError>;

constexpr Error errCode(Error err) {
	return (err >> 59) & onMask<Error>(5);
}

struct ErrorInfo {
	const char *file = nullptr;
	int line = -1;
	Error errCode = Error(0);

	ErrorInfo() = default;

	ErrorInfo(Error err) {
		this->file = reinterpret_cast<const char*>(static_cast<uint64_t>(err & onMask<Error>(48)));
		this->line = static_cast<int>((err >> 48) & onMask<Error>(11));
		this->errCode = ox::errCode(err);
	}
};

static constexpr Error _errorTags(Error line, Error errCode) {
	line &= onMask<Error>(11);
	line <<= 48;
	errCode &= onMask<Error>(5);
	errCode <<= 59;
	return errCode | line;
}

static constexpr Error _error(const char *file, uint32_t line, Error errCode) {
	return Error(errCode ? reinterpret_cast<uint64_t>(file) | _errorTags(Error(line), errCode) : 0);
}

template<typename T>
struct ValErr {
	T value;
	Error error;

	constexpr ValErr() noexcept: error(0) {
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

