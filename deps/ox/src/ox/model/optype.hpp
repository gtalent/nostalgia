/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/assert.hpp>
#include <ox/std/error.hpp>
#include <ox/std/strops.hpp>

namespace ox {

namespace OpType {
	constexpr auto Read = "Read";
	constexpr auto Write = "Write";
	constexpr auto WriteDefinition = "WriteDefinition";
}

// empty default implementations of model functions

template<typename T, typename O>
constexpr Error modelRead(T*, O*) noexcept {
	return OxError(1, "Model: modelRead not implemented");
}

template<typename T, typename O>
constexpr Error modelWrite(T*, O*) noexcept {
	return OxError(1, "Model: modelWrite not implemented");
}

template<typename T, typename O>
constexpr Error modelWriteDefinition(T*, O*) noexcept {
	return OxError(1, "Model: modelWriteDefinition not implemented");
}

template<typename T, typename O>
constexpr Error model(T *io, O *obj) noexcept {
	if constexpr(ox_strcmp(T::opType(), OpType::Read) == 0) {
		return modelRead(io, obj);
	} else if constexpr(ox_strcmp(T::opType(), OpType::Write) == 0) {
		return modelWrite(io, obj);
	} else if constexpr(ox_strcmp(T::opType(), OpType::WriteDefinition) == 0) {
		return modelWriteDefinition(io, obj);
	} else {
		oxAssert(OxError(1), "Missing model function");
		return OxError(1);
	}
}

}
