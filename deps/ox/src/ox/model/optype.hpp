/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>

namespace ox {

enum class OpType {
	Read = 1,
	Write = 2,
	WriteDefinition = 3,
};

// empty default implementations of model functions

template<typename T, typename O>
ox::Error modelRead(T*, O*) {
	return OxError(1);
}

template<typename T, typename O>
ox::Error modelWrite(T*, O*) {
	return OxError(1);
}

template<typename T, typename O>
ox::Error modelWriteDefinition(T*, O*) {
	return OxError(1);
}

template<typename T, typename O>
ox::Error model(T *io, O *obj) {
	if constexpr(T::opType() == ox::OpType::Read) {
		return modelRead(io, obj);
	} else if constexpr(T::opType() == ox::OpType::Write) {
		return modelWrite(io, obj);
	} else if constexpr(T::opType() == ox::OpType::WriteDefinition) {
		return modelWriteDefinition(io, obj);
	}
	return OxError(1);
}

}
