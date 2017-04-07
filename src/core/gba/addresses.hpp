/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <ox/std/types.hpp>

/////////////////////////////////////////////////////////////////
// I/O Registers

#define REG_DISPCNT *((volatile uint32_t*) 0x04000000)

/////////////////////////////////////////////////////////////////
// background registers

// background control registers
#define REG_BG0CNT *((volatile uint32_t*) 0x04000008)
#define REG_BG1CNT *((volatile uint32_t*) 0x0400000a)
#define REG_BG2CNT *((volatile uint32_t*) 0x0400000c)
#define REG_BG3CNT *((volatile uint32_t*) 0x0400000e)

// background horizontal scrolling registers
#define REG_BG0HOFS *((volatile uint32_t*) 0x04000010)
#define REG_BG1HOFS *((volatile uint32_t*) 0x04000014)
#define REG_BG2HOFS *((volatile uint32_t*) 0x04000018)
#define REG_BG3HOFS *((volatile uint32_t*) 0x0400001c)

// background vertical scrolling registers
#define REG_BG0VOFS *((volatile uint32_t*) 0x04000012)
#define REG_BG1VOFS *((volatile uint32_t*) 0x04000016)
#define REG_BG2VOFS *((volatile uint32_t*) 0x0400001a)
#define REG_BG3VOFS *((volatile uint32_t*) 0x0400001e)


/////////////////////////////////////////////////////////////////
// Memory Addresses

#define MEM_PALLETE_BG     *((unsigned short*) 0x05000000)
#define MEM_PALLETE_SPRITE *((unsigned short*) 0x05000200)

typedef uint16_t BgMapTile[1024];
#define MEM_BG_MAP ((BgMapTile*) 0x06000000)

#define MEM_ROM *((uint8_t*) 0x08000000)
