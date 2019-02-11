/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ox/std/error.hpp"

namespace ox {

enum class OpType {
	Read = 1,
	Write = 2,
	WriteDefinition = 3,
};

template<typename T, typename O>
ox::Error ioOp(T *io, O *obj) {
	if (io->opType() == ox::OpType::Read) {
		return ioOpRead(io, obj);
	} else {
		return ioOpWrite(io, obj);
	}
}

}
