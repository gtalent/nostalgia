//
// Copyright 2016 - 2020 gary@drinkingtea.net
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

.global nostalgia_core_intrwait
.type nostalgia_core_intrwait, %function
nostalgia_core_intrwait:
	swi 4
	bx lr

.global nostalgia_core_vblankintrwait
.type nostalgia_core_vblankintrwait, %function
nostalgia_core_vblankintrwait:
	swi 5
	bx lr

// vim: ft=armv4
