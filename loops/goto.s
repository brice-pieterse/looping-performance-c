	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 13, 0	sdk_version 13, 3
	.globl	_sum_all_goto                   ; -- Begin function sum_all_goto
	.p2align	2
_sum_all_goto:                          ; @sum_all_goto
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	str	x0, [sp, #24]
	str	x1, [sp, #16]
	str	xzr, [sp, #8]
	ldr	x8, [sp, #24]
	str	x8, [sp]
	b	LBB0_1
LBB0_1:                                 ; =>This Inner Loop Header: Depth=1
	ldr	x9, [sp]
	ldr	x8, [sp, #8]
	add	x8, x8, x9
	str	x8, [sp, #8]
	ldr	x8, [sp]
	add	x8, x8, #1
	str	x8, [sp]
	ldr	x8, [sp]
	ldr	x9, [sp, #16]
	subs	x8, x8, x9
	cset	w8, gt
	tbnz	w8, #0, LBB0_3
	b	LBB0_2
LBB0_2:                                 ;   in Loop: Header=BB0_1 Depth=1
	b	LBB0_1
LBB0_3:
	ldr	x0, [sp, #8]
	add	sp, sp, #32
	ret
	.cfi_endproc
                                        ; -- End function
.subsections_via_symbols
