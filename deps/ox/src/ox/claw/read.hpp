/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/mc/read.hpp>
#ifdef OX_USE_STDLIB
#include <ox/oc/read.hpp>
#endif
#include <ox/std/buffer.hpp>
#include <ox/std/string.hpp>

#include "format.hpp"

namespace ox {

namespace detail {

struct ClawHeader {
	String typeName;
	int typeVersion = -1;
	ClawFormat fmt = ClawFormat::None;
	const char *data = nullptr;
	std::size_t dataSize = 0;
};

Result<ClawHeader> readHeader(const char *buff, std::size_t buffLen) noexcept;

}

Result<Buffer> stripClawHeader(const char *buff, std::size_t buffLen) noexcept;

template<typename T>
Error readClaw(const char *buff, std::size_t buffLen, T *val) {
	oxRequire(header, detail::readHeader(buff, buffLen));
	switch (header.fmt) {
		case ClawFormat::Metal:
		{
			MetalClawReader reader(bit_cast<uint8_t*>(header.data), buffLen);
			return model(&reader, val);
		}
		case ClawFormat::Organic:
		{
#ifdef OX_USE_STDLIB
			OrganicClawReader reader(bit_cast<uint8_t*>(header.data), buffLen);
			return model(&reader, val);
#else
			break;
#endif
		}
		case ClawFormat::None:
			return OxError(1);
	}
	return OxError(1);
}

template<typename T>
Result<T> readClaw(const char *buff, std::size_t buffLen) {
	T val;
	oxReturnError(readClaw(buff, buffLen, &val));
	return move(val);
}

template<typename T>
Result<T> readClaw(const Buffer &buff) {
	return readClaw<T>(buff.data(), buff.size());
}

}
