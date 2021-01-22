/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
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

enum DispCtl {
	DispCtl_Mode0 = 0,
	DispCtl_Mode1 = 1,
	DispCtl_Mode2 = 2,
	DispCtl_Mode3 = 3,
	DispCtl_Mode4 = 4,
	DispCtl_Mode5 = 5,

	DispCtl_SpriteMap1D = 1 << 6,

	DispCtl_Bg0 = 1 << 8,
	DispCtl_Bg1 = 1 << 9,
	DispCtl_Bg2 = 1 << 10,
	DispCtl_Bg3 = 1 << 11,

	DispCtl_Obj = 1 << 12,
};

struct OX_ALIGN8 GbaSpriteAttrUpdate {
	uint16_t attr0 = 0;
	uint16_t attr1 = 0;
	uint16_t attr2 = 0;
	uint16_t idx = 0;

};

extern volatile uint16_t g_spriteUpdates;
extern GbaSpriteAttrUpdate g_spriteBuffer[config::GbaSpriteBufferLen];

}
