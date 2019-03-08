/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>
#include <ox/std/types.hpp>

namespace ox {

class FieldPresenceIndicator {
	private:
		uint8_t *m_mask = nullptr;
		int m_maskLen = 0;
		int m_fields = 0;

	public:
		FieldPresenceIndicator(uint8_t *mask, std::size_t maxLen);

		bool get(int i) const;

		Error set(int i, bool on);

		constexpr void setFields(int) noexcept;

		constexpr int getFields() const noexcept;

		constexpr void setMaxLen(int) noexcept;

		constexpr int getMaxLen() const noexcept;

};

constexpr void FieldPresenceIndicator::setFields(int fields) noexcept {
	m_fields = fields;
}

constexpr int FieldPresenceIndicator::getFields() const noexcept {
	return m_fields;
}

constexpr void FieldPresenceIndicator::setMaxLen(int maxLen) noexcept {
	m_maskLen = maxLen;
}

constexpr int FieldPresenceIndicator::getMaxLen() const noexcept {
	return m_maskLen;
}

}
