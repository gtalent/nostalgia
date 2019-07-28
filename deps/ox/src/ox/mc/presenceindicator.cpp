/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
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

bool FieldPresenceIndicator::get(int i) const {
	if (i / 8 < m_maskLen) {
		return (m_mask[i / 8] >> (i % 8)) & 1;
	} else {
		return MC_PRESENCEMASKOUTBOUNDS;
	}
}

Error FieldPresenceIndicator::set(int i, bool on) {
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
}

int FieldPresenceIndicator::getFields() const noexcept {
	return m_fields;
}

void FieldPresenceIndicator::setMaxLen(int maxLen) noexcept {
	m_maskLen = maxLen;
}

int FieldPresenceIndicator::getMaxLen() const noexcept {
	return m_maskLen;
}

}
