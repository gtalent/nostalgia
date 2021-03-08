/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "stddef.hpp"
#include "types.hpp"

namespace ox {

using RandomSeed = uint64_t[2];

class OX_PACKED Random {
	private:
		RandomSeed m_seed;

	public:
		Random();

		explicit Random(RandomSeed seed);

		uint64_t gen();
};

}
