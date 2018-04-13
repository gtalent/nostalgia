/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/byteswap.hpp>
#include "err.hpp"
#include "presencemask.hpp"

namespace ox {

FieldPresenseMask::FieldPresenseMask(uint8_t *mask, std::size_t maxLen) {
	m_mask = mask;
	m_maxLen = maxLen;
}

bool FieldPresenseMask::get(int i) {
	if (i / 8 < m_maxLen) {
		return (m_mask[i / 8] >> (i % 8)) & 1;
	} else {
		return MC_PRESENCEMASKOUTBOUNDS;
	}
}

int FieldPresenseMask::set(int i, bool on) {
	if (i / 8 < m_maxLen) {
		if (on) {
			m_mask[i / 8] |= 1 << (i % 8);
		} else {
			m_mask[i / 8] &= ~(1 << (i % 8));
		}
		return 0;
	} else {
		return MC_PRESENCEMASKOUTBOUNDS;
	}
}

void FieldPresenseMask::setMaxLen(int maxLen) {
	m_maxLen = maxLen;
}

int FieldPresenseMask::getMaxLen() {
	return m_maxLen;
}

}
