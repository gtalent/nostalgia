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

struct BaseError {
	const char *msg = nullptr;
	const char *file = "";
	uint16_t line = 0;

	BaseError() = default;

	constexpr BaseError(const BaseError &o) noexcept {
		msg = o.msg;
		file = o.file;
		line = o.line;
	}

	constexpr BaseError operator=(const BaseError &o) noexcept {
		msg = o.msg;
		file = o.file;
		line = o.line;
		return *this;
	}

};

using Error = Integer<uint64_t, BaseError>;

static constexpr Error _error(const char *file, uint32_t line, uint64_t errCode, const char *msg = nullptr) {
	Error err = static_cast<ox::Error>(errCode);
		err.file = file;
		err.line = line;
		err.msg = msg;
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
[[nodiscard]] constexpr ox::Error toError(const ox::ValErr<T> &ve) noexcept {
	return ve.error;
}

}

}

inline void oxIgnoreError(ox::Error) {}
#define oxReturnError(x) if (const auto _ox_error = ox::error::toError(x)) return _ox_error
#define oxThrowError(x) if (const auto _ox_error = ox::error::toError(x)) throw _ox_error

