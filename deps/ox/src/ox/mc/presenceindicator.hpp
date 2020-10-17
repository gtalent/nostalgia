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
		std::size_t m_maskLen = 0;
		std::size_t m_fields = 0;

	public:
		FieldPresenceIndicator(uint8_t *mask, std::size_t maxLen);

		Result<bool> get(std::size_t i) const;

		Error set(std::size_t i, bool on);

		void setFields(int) noexcept;

		int getFields() const noexcept;

		void setMaxLen(int) noexcept;

		int getMaxLen() const noexcept;

};

}
