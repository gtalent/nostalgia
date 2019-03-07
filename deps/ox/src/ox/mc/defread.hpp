/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "deftypes.hpp"
#include "read.hpp"

namespace ox::mc {


class MetalClawDefReader: public MetalClawReader {
	private:
		TypeStore m_typeStore;

	public:
		MetalClawDefReader(uint8_t *buff, std::size_t buffLen);

		const mc::TypeStore &typeStore() const;

};

template<typename T>
int readMCDef(uint8_t *buff, std::size_t buffLen, T *val) {
	MetalClawDefReader reader(buff, buffLen);
	return ioOp(&reader, val);
}

}
