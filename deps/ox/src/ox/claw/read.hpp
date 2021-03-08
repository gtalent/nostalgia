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
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

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

Result<Vector<char>> stripClawHeader(const char *buff, std::size_t buffLen) noexcept;

template<typename T>
Error readClaw(char *buff, std::size_t buffLen, T *val) {
	auto header = detail::readHeader(buff, buffLen);
	oxReturnError(header);
	switch (header.value.fmt) {
		case ClawFormat::Metal:
		{
			MetalClawReader reader(bit_cast<uint8_t*>(header.value.data), buffLen);
			return model(&reader, val);
		}
#ifdef OX_USE_STDLIB
		case ClawFormat::Organic:
		{
			OrganicClawReader reader(bit_cast<uint8_t*>(header.value.data), buffLen);
			return model(&reader, val);
		}
#endif
		case ClawFormat::None:
			return OxError(1);
	}
	return OxError(1);
}

}
