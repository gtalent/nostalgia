/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "desctypes.hpp"

namespace ox {

template<typename ReaderBase>
class TypeDescReader: public ReaderBase {
	private:
		TypeStore m_typeStore;

	public:
		TypeDescReader(uint8_t *buff, std::size_t buffLen);

		const TypeStore &typeStore() const;

};

template<typename ReaderBase>
TypeDescReader<ReaderBase>::TypeDescReader(uint8_t *buff, std::size_t buffLen): ReaderBase(buff, buffLen) {
}

template<typename ReaderBase>
const TypeStore &TypeDescReader<ReaderBase>::typeStore() const {
	return m_typeStore;
}

template<typename ReaderBase, typename T>
int readMCDef(uint8_t *buff, std::size_t buffLen, T *val) {
	TypeDescReader<ReaderBase> reader(buff, buffLen);
	return model(&reader, val);
}

}
