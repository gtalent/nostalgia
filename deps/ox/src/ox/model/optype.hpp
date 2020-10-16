/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
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
Error modelRead(T*, O*) {
	return OxError(1, "Model: modelRead not implemented");
}

template<typename T, typename O>
Error modelWrite(T*, O*) {
	return OxError(1, "Model: modelWrite not implemented");
}

template<typename T, typename O>
Error modelWriteDefinition(T*, O*) {
	return OxError(1, "Model: modelWriteDefinition not implemented");
}

template<typename T, typename O>
Error model(T *io, O *obj) {
	ox::Error err;
	if constexpr(ox_strcmp(T::opType(), ox::OpType::Read) == 0) {
		err = modelRead(io, obj);
	} else if constexpr(ox_strcmp(T::opType(), ox::OpType::Write) == 0) {
		err = modelWrite(io, obj);
	} else if constexpr(ox_strcmp(T::opType(), ox::OpType::WriteDefinition) == 0) {
		err = modelWriteDefinition(io, obj);
	}
	oxAssert(err, "Missing model function");
	return err;
}

}
