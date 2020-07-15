//
// Copyright 2016 - 2020 gtalent2@gmail.com
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//

.section .iwram, "ax", %progbits
.thumb
.align

.global nostalgia_core_halt
.type nostalgia_core_halt, %function
nostalgia_core_halt:
	swi 2
	bx lr

.global nostalgia_core_stop
.type nostalgia_core_stop, %function
nostalgia_core_stop:
	swi 3
	bx lr

.global nostalgia_core_wfi
.type nostalgia_core_wfi, %function
nostalgia_core_wfi:
	swi 4
	bx lr

.global nostalgia_core_vblankwfi
.type nostalgia_core_vblankwfi, %function
nostalgia_core_vblankwfi:
	swi 5
	bx lr

// vim: ft=armv4
