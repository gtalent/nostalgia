/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace ox {

class FieldPresenseMask {
	private:
		uint8_t *m_mask;
		int m_maxLen = 0;

	public:
		FieldPresenseMask(uint8_t *mask, size_t maxLen);

		bool get(int i);

		int set(int i, bool on);

		void setMaxLen(int);

		int getMaxLen();
};

}
