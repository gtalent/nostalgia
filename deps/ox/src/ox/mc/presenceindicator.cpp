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

FieldPresenceIndicator::FieldPresenceIndicator(uint8_t *mask, std::size_t maxLen) {
	m_mask = mask;
	m_maskLen = maxLen;
}

Result<bool> FieldPresenceIndicator::get(std::size_t i) const {
	if (i / 8 < m_maskLen) {
		return (m_mask[i / 8] >> (i % 8)) & 1;
	} else {
		return OxError(MC_PRESENCEMASKOUTBOUNDS);
	}
}

Error FieldPresenceIndicator::set(std::size_t i, bool on) {
	if (i / 8 < m_maskLen) {
		if (on) {
			m_mask[i / 8] |= 1 << (i % 8);
		} else {
			m_mask[i / 8] &= ~(1 << (i % 8));
		}
		return OxError(0);
	} else {
		return OxError(MC_PRESENCEMASKOUTBOUNDS);
	}
}

void FieldPresenceIndicator::setFields(int fields) noexcept {
	m_fields = fields;
	m_maskLen = (fields / 8 + 1) - (fields % 8 == 0);
}

	void FieldPresenceIndicator::setMaxLen(int maxLen) noexcept {
	m_maskLen = maxLen;
}

int FieldPresenceIndicator::getMaxLen() const noexcept {
	return m_maskLen;
}

}
