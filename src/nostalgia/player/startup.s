	.section ".crt0","ax"
	.global _start
	.align

	.arm
	.cpu arm7tdmi

_start:
	// disable interrupts
	//cpsid if
	b _reset
	.fill 156,1,0 // make room for Nintendo logo
	.fill 16,1,0
	.byte 0x30,0x31


_initCpp:
	// copy data
	ldr r0,=__data_lma
	ldr r1,=__data_start__
	ldr r2,=__data_end__
	// loop over data until copied
data_copy_loop:
	cmp r1,r2
	ldmltia r0!,{r3}
	stmltia r1!,{r3}
	blt data_copy_loop

_reset:
	ldr r0,=_reset
	ldr r1,=_initCpp
	mov lr,r1
	ldr sp,=__ewram_end
	b _initCpp

	.size _start, . - _start
	.end

// vim:ft=arm
