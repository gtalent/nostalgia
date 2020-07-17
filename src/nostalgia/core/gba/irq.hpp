/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

namespace nostalgia::core {

constexpr uint16_t Int_vblank = 1 << 0;
constexpr uint16_t Int_hblank = 1 << 1;
constexpr uint16_t Int_vcount = 1 << 2;
constexpr uint16_t Int_timer0 = 1 << 3;
constexpr uint16_t Int_timer1 = 1 << 4;
constexpr uint16_t Int_timer2 = 1 << 5;
constexpr uint16_t Int_timer3 = 1 << 6;
constexpr uint16_t Int_serial = 1 << 7; // link cable
constexpr uint16_t Int_dma0   = 1 << 8;
constexpr uint16_t Int_dma1   = 1 << 9;
constexpr uint16_t Int_dma2   = 1 << 10;
constexpr uint16_t Int_dma3   = 1 << 11;
constexpr uint16_t Int_dma4   = 1 << 12;
constexpr uint16_t Int_dma5   = 1 << 13;
constexpr uint16_t Int_input  = 1 << 14; // gamepad
constexpr uint16_t Int_cart   = 1 << 15; // cartridge removed

}
