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

// empty default implementations of ioOp functions

template<typename T, typename O>
ox::Error ioOpRead(T*, O*) {
	return OxError(1);
}

template<typename T, typename O>
ox::Error ioOpWrite(T*, O*) {
	return OxError(1);
}

template<typename T, typename O>
ox::Error ioOpWriteDefinition(T*, O*) {
	return OxError(1);
}

template<typename T, typename O>
ox::Error ioOp(T *io, O *obj) {
	if constexpr (T::opType() == ox::OpType::Read) {
		return ioOpRead(io, obj);
	} else if constexpr (T::opType() == ox::OpType::Write) {
		return ioOpWrite(io, obj);
	} else if constexpr (T::opType() == ox::OpType::WriteDefinition) {
		return ioOpWriteDefinition(io, obj);
	}
	return OxError(1);
}

}
