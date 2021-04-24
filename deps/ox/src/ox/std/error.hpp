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

using ErrorCode = uint16_t;

struct [[nodiscard]] Error {
	const char *msg = nullptr;
	const char *file = nullptr;
	uint16_t line = 0;
	ErrorCode errCode = 0;

	explicit constexpr Error(ErrorCode ec = 0) noexcept: errCode(ec) {
	}

	explicit constexpr Error(const char *file, uint32_t line, ErrorCode errCode, const char *msg = nullptr) noexcept {
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

	using type = typename ox::remove_reference<T>::type;

	T value;
	Error error;

	constexpr Result() noexcept: error(0) {
	}

	constexpr Result(const Error &error) noexcept: error(error) {
	}

	constexpr Result(const type &value, const Error &error = OxError(0)) noexcept: value(const_cast<type&>(value)), error(error) {
	}

	constexpr Result(type &&value, const Error &error = OxError(0)) noexcept: value(ox::move(value)), error(error) {
	}

	explicit constexpr operator const type&() const noexcept {
		return value;
	}

	explicit constexpr operator type&() noexcept {
		return value;
	}

	[[nodiscard]]
	constexpr bool ok() const noexcept {
		return error == 0;
	}

	constexpr Error get(type *val) const noexcept {
		*val = value;
		return error;
	}

	constexpr Error get(type *val) noexcept {
		*val = value;
		return error;
	}

	constexpr Error moveTo(type *val) noexcept {
		*val = ox::move(value);
		return error;
	}

};

namespace detail {

constexpr Error toError(const Error &e) noexcept {
	return e;
}

template<typename T>
constexpr Error toError(const Result<T> &ve) noexcept {
	return ve.error;
}

}

}

constexpr void oxIgnoreError(ox::Error) noexcept {}
#if __cplusplus >= 202002L
#define oxReturnError(x) if (const auto _ox_error = ox::detail::toError(x)) [[unlikely]] return _ox_error
#define oxThrowError(x) if (const auto _ox_error = ox::detail::toError(x)) [[unlikely]] throw _ox_error
#else
#define oxReturnError(x) if (const auto _ox_error = ox::detail::toError(x)) return _ox_error
#define oxThrowError(x) if (const auto _ox_error = ox::detail::toError(x)) throw _ox_error
#endif
#define oxConcatImpl(a, b) a##b
#define oxConcat(a, b) oxConcatImpl(a, b)
// oxRequire Mutable
#define oxRequireM(out, x) auto [out, oxConcat(oxRequire_err_, __LINE__)] = x; oxReturnError(oxConcat(oxRequire_err_, __LINE__))
#define oxRequire(out, x) const oxRequireM(out, x)
// oxRequire Mutable Throw
#define oxRequireMT(out, x) auto [out, oxConcat(oxRequire_err_, __LINE__)] = x; oxThrowError(oxConcat(oxRequire_err_, __LINE__))
// oxRequire Throw
#define oxRequireT(out, x) const oxRequireMT(out, x)

