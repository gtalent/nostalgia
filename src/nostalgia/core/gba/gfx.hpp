/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/stddef.hpp>
#include <ox/std/types.hpp>

#include <nostalgia/core/config.hpp>

namespace nostalgia::core {

struct OX_ALIGN8 GbaSpriteAttrUpdate {
	uint16_t attr0 = 0;
	uint16_t attr1 = 0;
	uint16_t attr2 = 0;
	uint16_t idx = 0;

};

extern volatile uint16_t g_spriteUpdates;
extern GbaSpriteAttrUpdate g_spriteBuffer[config::GbaSpriteBufferLen];

}
