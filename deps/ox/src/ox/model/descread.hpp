/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
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
		TypeDescReader(uint8_t *buff, std::size_t buffLen) noexcept;

		const TypeStore &typeStore() const noexcept;

};

template<typename ReaderBase>
TypeDescReader<ReaderBase>::TypeDescReader(uint8_t *buff, std::size_t buffLen) noexcept: ReaderBase(buff, buffLen) {
}

template<typename ReaderBase>
const TypeStore &TypeDescReader<ReaderBase>::typeStore() const noexcept {
	return m_typeStore;
}

template<typename ReaderBase, typename T>
int readMCDef(uint8_t *buff, std::size_t buffLen, T *val) noexcept {
	TypeDescReader<ReaderBase> reader(buff, buffLen);
	return model(&reader, val);
}

}
