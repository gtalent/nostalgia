/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "read.hpp"

namespace ox {

namespace detail {

Result<ClawHeader> readHeader(const char *buff, std::size_t buffLen) noexcept {
	const auto s1End = ox_strchr(buff, ';', buffLen);
	if (!s1End) {
		return OxError(1);
	}
	const auto s1Size = s1End - buff;
	String fmt(buff, s1Size);
	buff += s1Size + 1;
	buffLen -= s1Size + 1;

	const auto s2End = ox_strchr(buff, ';', buffLen);
	if (!s2End) {
		return OxError(1);
	}
	const auto s2Size = s2End - buff;
	String typeName(buff, s2Size);
	buff += s2Size + 1;
	buffLen -= s2Size + 1;

	const auto s3End = ox_strchr(buff, ';', buffLen);
	if (!s3End) {
		return OxError(1);
	}
	const auto s3Size = s3End - buff;
	String versionStr(buff, s3Size);
	buff += s3Size + 1;
	buffLen -= s3Size + 1;

	ClawHeader hdr;
	if (fmt == "M1") {
		hdr.fmt = ClawFormat::Metal;
	} else if (fmt == "O1") {
		hdr.fmt = ClawFormat::Organic;
	} else {
		return OxError(1);
	}
	hdr.typeName = typeName;
	if (auto r = ox_atoi(versionStr.c_str()); r.error == 0) {
		hdr.typeVersion = r.value;
	}
	hdr.data = buff;
	hdr.dataSize = buffLen;
	return hdr;
}

}

Result<Buffer> stripClawHeader(const char *buff, std::size_t buffLen) noexcept {
	auto header = detail::readHeader(buff, buffLen);
	oxReturnError(header);
	Buffer out(header.value.dataSize);
	ox_memcpy(out.data(), header.value.data, out.size());
	return ox::move(out);
}

}
