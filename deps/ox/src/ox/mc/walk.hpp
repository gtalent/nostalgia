/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/model/walk.hpp>

namespace ox {

template<typename Reader, typename Handler>
ox::Error walkMC(DescriptorType *type, uint8_t *data, std::size_t dataLen, Handler handler) {
	DataWalker<Reader, Handler> walker(type, handler);
	Reader rdr(data, dataLen);
	return model(&rdr, &walker);
}

}
