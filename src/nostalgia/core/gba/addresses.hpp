/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

/////////////////////////////////////////////////////////////////
// Interrupt Handler

using interrupt_handler = void (*)(void);
#define REG_ISR *reinterpret_cast<interrupt_handler*>(0x03007FFC)
#define REG_IE  *reinterpret_cast<volatile uint16_t*>(0x04000200)
#define REG_IF  *reinterpret_cast<volatile uint16_t*>(0x04000202)
#define REG_IME *reinterpret_cast<volatile uint16_t*>(0x04000208)

/////////////////////////////////////////////////////////////////
// Display Registers

#define REG_DISPCTL  *reinterpret_cast<volatile uint32_t*>(0x04000000)
#define REG_DISPSTAT *reinterpret_cast<volatile uint32_t*>(0x04000004)
#define REG_VCOUNT   *reinterpret_cast<volatile uint32_t*>(0x04000006)

/////////////////////////////////////////////////////////////////
// Timers

#define REG_TIMER0    *reinterpret_cast<volatile uint16_t*>(0x04000100)
#define REG_TIMER0CTL *reinterpret_cast<volatile uint16_t*>(0x04000102)

#define REG_TIMER1    *reinterpret_cast<volatile uint16_t*>(0x04000104)
#define REG_TIMER1CTL *reinterpret_cast<volatile uint16_t*>(0x04000106)

#define REG_TIMER2    *reinterpret_cast<volatile uint16_t*>(0x04000108)
#define REG_TIMER2CTL *reinterpret_cast<volatile uint16_t*>(0x0400010a)

#define REG_TIMER3    *reinterpret_cast<volatile uint16_t*>(0x0400010c)
#define REG_TIMER3CTL *reinterpret_cast<volatile uint16_t*>(0x0400010e)

/////////////////////////////////////////////////////////////////
// background registers

// background control registers
#define REG_BG0CTL *reinterpret_cast<volatile uint32_t*>(0x04000008)
#define REG_BG1CTL *reinterpret_cast<volatile uint32_t*>(0x0400000a)
#define REG_BG2CTL *reinterpret_cast<volatile uint32_t*>(0x0400000c)
#define REG_BG3CTL *reinterpret_cast<volatile uint32_t*>(0x0400000e)

// background horizontal scrolling registers
#define REG_BG0HOFS *reinterpret_cast<volatile uint32_t*>(0x04000010)
#define REG_BG1HOFS *reinterpret_cast<volatile uint32_t*>(0x04000014)
#define REG_BG2HOFS *reinterpret_cast<volatile uint32_t*>(0x04000018)
#define REG_BG3HOFS *reinterpret_cast<volatile uint32_t*>(0x0400001c)

// background vertical scrolling registers
#define REG_BG0VOFS *reinterpret_cast<volatile uint32_t*>(0x04000012)
#define REG_BG1VOFS *reinterpret_cast<volatile uint32_t*>(0x04000016)
#define REG_BG2VOFS *reinterpret_cast<volatile uint32_t*>(0x0400001a)
#define REG_BG3VOFS *reinterpret_cast<volatile uint32_t*>(0x0400001e)

/////////////////////////////////////////////////////////////////
// User Input

#define REG_GAMEPAD *reinterpret_cast<volatile uint16_t*>(0x04000130)

/////////////////////////////////////////////////////////////////
// Memory Addresses

#define MEM_EWRAM_BEGIN reinterpret_cast<uint8_t*>(0x02000000)
#define MEM_EWRAM_END   reinterpret_cast<uint8_t*>(0x0203FFFF)

#define MEM_IWRAM_BEGIN reinterpret_cast<uint8_t*>(0x03000000)
#define MEM_IWRAM_END   reinterpret_cast<uint8_t*>(0x03007FFF)

#define REG_BLNDCTL *reinterpret_cast<uint16_t*>(0x04000050)

#define MEM_BG_PALETTE reinterpret_cast<uint16_t*>(0x05000000)
#define MEM_SPRITE_PALETTE reinterpret_cast<uint16_t*>(0x05000200)

typedef uint16_t BgMapTile[1024];
#define MEM_BG_TILES reinterpret_cast<BgMapTile*>(0x06000000)
#define MEM_BG_MAP reinterpret_cast<BgMapTile*>(0x0600e000)

#define MEM_SPRITE_TILES reinterpret_cast<uint8_t*>(0x06010000)
#define MEM_OAM reinterpret_cast<uint64_t*>(0x07000000)

#define MEM_ROM reinterpret_cast<char*>(0x08000000)

#define MEM_SRAM reinterpret_cast<char*>(0x0e000000)
#define MEM_SRAM_SIZE 65535
