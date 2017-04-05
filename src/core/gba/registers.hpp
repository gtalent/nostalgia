/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef NOSTALGIA_CORE_GBA_REGISTERS_HPP
#define NOSTALGIA_CORE_GBA_REGISTERS_HPP

#define REG_DISPCNT *((volatile unsigned int*) 0x04000000)

/////////////////////////////////////////////////////////////////
// background registers

// control registers
#define REG_BG0CNT *((volatile unsigned int*) 0x04000008)
#define REG_BG1CNT *((volatile unsigned int*) 0x0400000a)
#define REG_BG2CNT *((volatile unsigned int*) 0x0400000c)
#define REG_BG3CNT *((volatile unsigned int*) 0x0400000e)

// horizontal scrolling registers
#define REG_BG0HOFS *((volatile unsigned int*) 0x04000010)
#define REG_BG1HOFS *((volatile unsigned int*) 0x04000014)
#define REG_BG2HOFS *((volatile unsigned int*) 0x04000018)
#define REG_BG3HOFS *((volatile unsigned int*) 0x0400001a)

// vertical scrolling registers
#define REG_BG0VOFS *((volatile unsigned int*) 0x04000012)
#define REG_BG1VOFS *((volatile unsigned int*) 0x04000016)
#define REG_BG2VOFS *((volatile unsigned int*) 0x0400001a)
#define REG_BG3VOFS *((volatile unsigned int*) 0x0400001c)

#endif
