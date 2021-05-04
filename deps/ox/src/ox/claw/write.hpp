/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/mc/write.hpp>
#ifdef OX_USE_STDLIB
#include <ox/oc/write.hpp>
#endif
#include <ox/std/buffer.hpp>
#include <ox/std/string.hpp>

#include "format.hpp"

namespace ox {

namespace detail {

struct TypeInfoCatcher {

	const char *name = nullptr;

	template<typename T = void>
	constexpr void setTypeInfo(const char *name = T::TypeName, int = T::Fields) noexcept {
		this->name = name;
	}

	constexpr Error field(...) noexcept {
		return OxError(0);
	}

	static constexpr auto opType() {
		return OpType::Write;
	}

};

template<typename T, typename = int>
struct type_version {
	static constexpr auto value = -1;
};

template<typename T>
struct type_version<T, decltype((void) T::TypeVersion, -1)> {
	static constexpr auto value = T::TypeVersion;
};

template<typename T>
constexpr const char *getTypeName(T *t) noexcept {
	TypeInfoCatcher tnc;
	oxIgnoreError(model(&tnc, t));
	return tnc.name;
}

template<typename T>
Result<String> writeClawHeader(T *t, ClawFormat fmt) noexcept {
	String out;
	switch (fmt) {
		case ClawFormat::Metal:
			out += "M1;";
			break;
		case ClawFormat::Organic:
			out += "O1;";
			break;
		default:
			return OxError(1);
	}
	out += detail::getTypeName(t);
	out += ";";
	const auto tn = detail::type_version<T>::value;
	if (tn > -1) {
		out += tn;
	}
	out += ";";
	return out;
}

}

template<typename T>
Result<Buffer> writeClaw(T *t, ClawFormat fmt) {
	oxRequire(header, detail::writeClawHeader(t, fmt));
	oxRequire(data, fmt == ClawFormat::Metal ? writeMC(t) : writeOC(t));
	Buffer out(header.len() + data.size());
	memcpy(out.data(), header.data(), header.len());
	memcpy(out.data() + header.len(), data.data(), data.size());
	return out;
}

}
