/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

uint64_t ox_rand();

namespace ox {

typedef uint64_t RandomSeed[2];

class Random {
	public:
		static RandomSeed DEFAULT_SEED;

	private:
		RandomSeed m_seed;

	public:
		Random(RandomSeed seed = DEFAULT_SEED);

		uint64_t gen();
};

}
