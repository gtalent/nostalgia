/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
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

#define OxError(...) ox::Error(__FILE__, __LINE__, __VA_ARGS__)

namespace ox {

struct [[nodiscard]] Error {
	const char *msg = nullptr;
	const char *file = nullptr;
	uint16_t line = 0;
	uint64_t errCode = 0;

	explicit constexpr Error(uint64_t ec = 0) noexcept: errCode(ec) {
	}

	explicit constexpr Error(const char *file, uint32_t line, uint64_t errCode, const char *msg = nullptr) noexcept {
		this->file = file;
		this->line = line;
		this->msg = msg;
		this->errCode = errCode;
	}

	constexpr Error(const Error &o) noexcept {
		this->msg = o.msg;
		this->file = o.file;
		this->line = o.line;
		this->errCode = o.errCode;
	}

	constexpr Error &operator=(const Error &o) noexcept {
		this->msg = o.msg;
		this->file = o.file;
		this->line = o.line;
		this->errCode = o.errCode;
		return *this;
	}

	constexpr operator uint64_t() const noexcept {
		return errCode;
	}

};

template<typename T>
struct [[nodiscard]] Result {

	using type = T;

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

	constexpr Error moveTo(T *val) noexcept {
		*val = ox::move(value);
		return error;
	}

};

namespace detail {

constexpr Error toError(Error e) noexcept {
	return e;
}

template<typename T>
constexpr Error toError(const Result<T> &ve) noexcept {
	return ve.error;
}

}

}

inline void oxIgnoreError(ox::Error) noexcept {}
#if __cplusplus >= 202002L
#define oxReturnError(x) if (const auto _ox_error = ox::detail::toError(x)) [[unlikely]] return _ox_error
#define oxThrowError(x) if (const auto _ox_error = ox::detail::toError(x)) [[unlikely]] throw _ox_error
#else
#define oxReturnError(x) if (const auto _ox_error = ox::detail::toError(x)) return _ox_error
#define oxThrowError(x) if (const auto _ox_error = ox::detail::toError(x)) throw _ox_error
#endif
#define oxConcatImpl(a, b) a##b
#define oxConcat(a, b) oxConcatImpl(a, b)
#define oxRequire(out, x) auto [out, oxConcat(oxRequire_err_, __LINE__)] = x; oxReturnError(oxConcat(oxRequire_err_, __LINE__))

