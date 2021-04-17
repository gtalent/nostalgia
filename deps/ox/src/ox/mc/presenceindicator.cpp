/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/byteswap.hpp>
#include "err.hpp"
#include "presenceindicator.hpp"

namespace ox {

template class FieldBitmapReader<uint8_t*>;
template class FieldBitmapReader<const uint8_t*>;

FieldBitmap::FieldBitmap(uint8_t *mask, std::size_t maxLen) noexcept: FieldBitmapReader<uint8_t*>(mask, maxLen) {
}

Error FieldBitmap::set(std::size_t i, bool on) noexcept {
	if (i / 8 < m_mapLen) {
		if (on) {
			m_map[i / 8] |= 1 << (i % 8);
		} else {
			m_map[i / 8] &= ~(1 << (i % 8));
		}
		return OxError(0);
	} else {
		return OxError(MC_PRESENCEMASKOUTBOUNDS);
	}
}

}
