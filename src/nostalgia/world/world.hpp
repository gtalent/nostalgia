/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace nostalgia {
namespace world {

struct ZoneDef {
	int width = 0;
	int height = 0;
	uint16_t tileMap;
};

}
}