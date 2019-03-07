/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "defread.hpp"

namespace ox::mc {

MetalClawDefReader::MetalClawDefReader(uint8_t *buff, std::size_t buffLen): MetalClawReader(buff, buffLen) {
}

const mc::TypeStore &MetalClawDefReader::typeStore() const {
	return m_typeStore;
}

}
