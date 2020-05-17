/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/mc/write.hpp>
#include <ox/oc/write.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

#include "format.hpp"

namespace ox {

namespace detail {

struct TypeInfoCatcher {

	const char *name = nullptr;

	template<typename T>
	constexpr void setTypeInfo(const char *name = T::TypeName, int = T::Fields) noexcept {
		this->name = name;
	}

	constexpr ox::Error field(...) noexcept {
		return OxError(0);
	}

};

template<typename T>
[[nodiscard]] constexpr int getTypeVersion(int version = T::TypeVersion) noexcept {
	return version;
}

[[nodiscard]] constexpr int getTypeVersion(...) noexcept {
	return -1;
}

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
	model(&tnc, t);
	return tnc.name;
}

template<typename T>
ValErr<String> writeClawHeader(T *t, ClawFormat fmt) noexcept {
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
ValErr<Vector<char>> writeClaw(T *t, ClawFormat fmt) {
	auto [header, headerErr] = detail::writeClawHeader(t, fmt);
	oxReturnError(headerErr);
	const auto [data, dataErr] = fmt == ClawFormat::Metal ? writeMC(t) : writeOC(t);
	oxReturnError(dataErr);
	ox::Vector<char> out(header.len() + data.size());
	memcpy(out.data(), header.data(), header.len());
	memcpy(out.data() + header.len(), data.data(), data.size());
	return out;
}

}
