//
// Copyright 2016 - 2020 gary@drinkingtea.net
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//

.section .iwram, "ax", %progbits
.arm
.align

.extern nostalgia_core_isr_vblank
.extern nostalgia_core_isr_timer0
.extern nostalgia_core_isr_timer1
.extern nostalgia_core_isr_timer2
.extern nostalgia_core_isr_timer3

.equ REG_IFBIOS,         0x03007ff8

.equ REG_IE,             0x04000200
.equ REG_IF,             0x04000202
.equ REG_IME,            0x04000208

.equ Int_vblank,                  1
.equ Int_hblank,                  2
.equ Int_vcount,                  4
.equ Int_timer0,                  8
.equ Int_timer1,                 16
.equ Int_timer2,                 32
.equ Int_timer3,                 64
.equ Int_serial,                128 // link cable
.equ Int_dma0,                  256
.equ Int_dma1,                  512
.equ Int_dma2,                 1024
.equ Int_dma3,                 2048
.equ Int_dma4,                 4096
.equ Int_dma5,                 8192
.equ Int_input,               16384 // gamepad
.equ Int_cart,                32768 // cartridge removed


.global isr
.type isr, %function
isr:
	// read IE
	ldr r0, =#REG_IE
	ldrh r1, [r0] // r1 becomes IE value
	ldrh r2, [r0, #2] // r2 becomes IF value
	and r1, r1, r2 // r1 becomes IE & IF
	// done with r2 as IF value

	// Acknowledge IRQ in REG_IF
	strh r1, [r0, #2]
	ldr r0, =#REG_IFBIOS
	// Acknowledge IRQ in REG_IFBIOS
	ldr r2, [r0] // r2 becomes REG_IFBIOS value
	orr r2, r2, r1
	str r2, [r0]
	// done with r2 as IFBIOS value
	// done with r0 as REG_IFBIOS

	////////////////////////////////////////////////////
	// Interrupt Table Begin                          //
	////////////////////////////////////////////////////

	cmp r1, #Int_timer0
	ldreq r0, =nostalgia_core_isr_timer0
	beq isr_call_handler

	cmp r1, #Int_vblank
	ldreq r0, =nostalgia_core_isr_vblank
	beq isr_call_handler

	////////////////////////////////////////////////////
	// Interrupt Table End                            //
	////////////////////////////////////////////////////

	bx lr

isr_call_handler:
	// clear IME to disable interrupts
	ldr r2, =#REG_IME
	mov r1, #0
	str r1, [r2]

	// enter system mode
	mrs r1, cpsr
	bic r1, r1, #0xDF
	orr r1, r1, #0x1F
	msr cpsr, r1

	push {lr}
	ldr lr, =isr_restore
	bx r0

isr_restore:
	pop {lr}

	// re-enter irq mode
	mrs r0, cpsr
	bic r0, r0, #0xDF
	orr r0, r0, #0x92
	msr cpsr, r0

	// set IME to re-enable interrupts
	ldr r2, =#REG_IME
	mov r0, #1
	str r0, [r2]

	bx lr

// vim: ft=armv4
