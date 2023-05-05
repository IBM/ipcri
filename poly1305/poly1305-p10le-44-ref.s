# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2023- IBM Corp. All rights reserved
#
#===================================================================================
# Written by Danny Tsen <dtsen@us.ibm.com>
#
# This program uses 44, 44 and 40 bits limbs.  It has been tested and produces 
#   cocrrect output.
#
#
.machine "any"
.text

.macro	SAVE_GPR GPR OFFSET FRAME
	std	\GPR,\OFFSET(\FRAME)
.endm

.macro	SAVE_VRS VRS OFFSET FRAME
	li	16, \OFFSET
	stvx	\VRS, 16, \FRAME
.endm

.macro	SAVE_VSX VSX OFFSET FRAME
	li	16, \OFFSET
	stxvx	\VSX, 16, \FRAME
.endm

.macro	RESTORE_GPR GPR OFFSET FRAME
	ld	\GPR,\OFFSET(\FRAME)
.endm

.macro	RESTORE_VRS VRS OFFSET FRAME
	li	16, \OFFSET
	lvx	\VRS, 16, \FRAME
.endm

.macro	RESTORE_VSX VSX OFFSET FRAME
	li	16, \OFFSET
	lxvx	\VSX, 16, \FRAME
.endm

.macro SAVE_REGS
	mflr 0
	std 0, 16(1)
	stdu 1,-752(1)

	SAVE_GPR 14, 112, 1
	SAVE_GPR 15, 120, 1
	SAVE_GPR 16, 128, 1
	SAVE_GPR 17, 136, 1
	SAVE_GPR 18, 144, 1
	SAVE_GPR 19, 152, 1
	SAVE_GPR 20, 160, 1
	SAVE_GPR 21, 168, 1
	SAVE_GPR 22, 176, 1
	SAVE_GPR 23, 184, 1
	SAVE_GPR 24, 192, 1
	SAVE_GPR 25, 200, 1
	SAVE_GPR 26, 208, 1
	SAVE_GPR 27, 216, 1
	SAVE_GPR 28, 224, 1
	SAVE_GPR 29, 232, 1
	SAVE_GPR 30, 240, 1
	SAVE_GPR 31, 248, 1
.endm # SAVE_REGS

.macro RESTORE_REGS
	RESTORE_GPR 14, 112, 1
	RESTORE_GPR 15, 120, 1
	RESTORE_GPR 16, 128, 1
	RESTORE_GPR 17, 136, 1
	RESTORE_GPR 18, 144, 1
	RESTORE_GPR 19, 152, 1
	RESTORE_GPR 20, 160, 1
	RESTORE_GPR 21, 168, 1
	RESTORE_GPR 22, 176, 1
	RESTORE_GPR 23, 184, 1
	RESTORE_GPR 24, 192, 1
	RESTORE_GPR 25, 200, 1
	RESTORE_GPR 26, 208, 1
	RESTORE_GPR 27, 216, 1
	RESTORE_GPR 28, 224, 1
	RESTORE_GPR 29, 232, 1
	RESTORE_GPR 30, 240, 1
	RESTORE_GPR 31, 248, 1

	addi    1, 1, 752
	ld 0, 16(1)
	mtlr 0
.endm # RESTORE_REGS

#
# Init poly1305
#  v14, v15
#  vs30, vs31
#
Poly1305_init_44:
	#  mask 0x0FFFFFFC0FFFFFFC
	#  mask 0x0FFFFFFC0FFFFFFF
	ld	10, rmask@got(2)
	ld	11, 0(10)
	ld	12, 8(10)

	# load 44 bits masks
	li	17, 16
	li	18, 32
	lxvb16x 32+14, 17, 10	# 0xfffffffffff
	lxvb16x 32+15, 18, 10	# 0x3ffffffffff
	mfvsrld	14, 32+14
	mfvsrld	15, 32+15
	xxlor	10, 32+14, 32+14
	xxlor	11, 32+15, 32+15

	ld	10, cnum@got(2)
	lxv	32+16, 0(10)	# 0x2c - 44
	lxv	32+17, 16(10)	# 0x2a - 42

	# initialize
	# load key from r3
	ld	9, 0(3)
	ld	10, 8(3)
	and.	9, 9, 11	# cramp mask r0
	and.	10, 10, 12	# cramp mask r1

	# smash to  3 44 bits limbs (r0, r1, r2)
	and.	16, 9, 14	# r0 - & 0xfffffffffff
	srdi	17, 9, 44
	sldi	18, 10, 20
	or	17, 17, 18
	and.	17, 17, 14	# r1
	srdi	18, 10, 24	# r2

	# s1, s2
	li	22, 4
        sldi    21, 17, 2
        add     19, 21, 17              # s1: r19 - r1*5
	sldi	19, 19, 2
        sldi    21, 18, 2
        add     20, 21, 18              # s2: r20 - r2*5
	mulld	20, 20, 22

        # save r and s
	li	25, 0
	mtvsrdd 32+3, 16, 20	# r0, s2
	mtvsrdd 32+4, 19, 25	# s1
	mtvsrdd 32+5, 17, 16	# r1, r0
	mtvsrdd 32+6, 20, 25	# s2
	mtvsrdd 32+7, 18, 17	# r2, r1
	mtvsrdd 32+8, 16, 25	# r0

	blr

# Input: v12
# Output: v18, v19, v13
Smash_vec_44:
	# smash to 2 44 bits and 1 40 bits
	vand	18, 12, 14		# m0
	vsrq	13, 12, 16
	vand	19, 13, 14		# m1
	vsrq	13, 13, 16
	vand	13, 13, 14		# m2
	xxlor	32+12, 32+13, 0		# hibit (1<<40)
	blr

#
# carry reduction
# h %=p
#
# Input: v0, v1, v2
# Output: v0, v1, v2
#
#  The carry is less than 64 bits so use vaddudm.
#
Carry_reduction:
	vsrq	13, 0, 16	# h0 carry - >> 44
	vand	0, 0, 14	# h0

	vaddudm 1, 1, 13
	vsrq	13, 1, 16	# h1 carry - >> 44
	vand	1, 1, 14	# h1

	vaddudm 2, 2, 13
	vsrq	13, 2, 17	# h2 carry - >> 42
	vand	2, 2, 15	# h2 - & 0x3ffffffffff

	vspltisb 18, 2
	vslq	18, 13, 18
	vaddudm	13, 13, 18	# c * 5
	vaddudm 0, 0, 13
	vsrq	13, 0, 16	# h0 >> 44
	vaddudm	1, 1, 13	# h1+c
	vand	0, 0, 14	# h0

	blr

#
# poly1305 multiplication
# h *= r, h %= p
#	d0 = h0 * r0 + h1 * s2 + h2 * s1
#	d1 = h0 * r1 + h1 * r0 + h2 * s2
#       d2 = h0 * r2 + h1 * r1 + h2 * r0
#
#
# unsigned int poly1305_44(unsigned char *state, const byte *src, size_t len, highbit)
#
.global poly1305_44
.align 5
poly1305_44:
	cmpdi	5, 0
	blt	Out_no_poly

	SAVE_REGS

	# Init poly1305
	bl Poly1305_init_44

	li 25, 0			# offset to inp and outp

	add 11, 25, 4

	# load h
	ld	27, 32(3)
	ld	28, 40(3)
	ld	29, 48(3)
        mtvsrdd 32+0, 0, 27
        mtvsrdd 32+1, 0, 28
        mtvsrdd 32+2, 0, 29

        li      30, 16
        divdu   31, 5, 30

        mtctr   31

        mr      24, 6		# highbit
	sldi	24, 24, 40
        mtvsrdd 0, 0, 24	# vs0 : highbit = m2

Loop_block:

	vxor	9, 9, 9

	lxv	32+12, 0(11)	# m
	addi	11, 11, 16

	bl	Smash_vec_44

	vaddudm	0, 0, 18
	vaddudm	1, 1, 19
	vaddudm	2, 2, 12

	xxpermdi 32+0, 32+0, 32+1, 3
	xxpermdi 32+2, 32+2, 32+2, 2

	#
	# v10 = (h0, h1), v11 = h2
	# v3 = (r0, s2), v4 = s1, v5 = (r1, r0), v6 = s2, v7 = (r2, r1), v8 = r0
	#
	#	d0 = h0 * r0 + h1 * s2 + h2 * s1
	vmsumudm	18, 0, 3, 9		# h0 * r0, h1 * s2
	vmsumudm	12, 2, 4, 18		# d0 += h2 * s1

	#	d1 = h0 * r1 + h1 * r0 + h2 * s2
	vmsumudm	18, 0, 5, 9		# h0 * r1, h1 * r0
	vmsumudm	13, 2, 6, 18		# d1 += h2 * s2

	#       d2 = h0 * r2 + h1 * r1 + h2 * r0
	vmsumudm	18, 0, 7, 9		# h0 * r2, h1 * r1
	vmsumudm	19, 2, 8, 18		# d2 += h2 * r0

	vmr	0, 12
	vmr	1, 13
	vmr	2, 19

	# carry reduction
	# h %=p
	#  The carry is less than 64 bits so use vaddudm.
	#
	bl Carry_reduction

	bdnz	Loop_block

	mfvsrld	10, 32+0
	mfvsrld	12, 32+1
	mfvsrld	15, 32+2

Poly_out:
        std     10, 32(3)
        std     12, 40(3)
        std     15, 48(3)

	li	3, 0

	RESTORE_REGS

	blr

Out_no_poly:
	li	3, 0
	blr

#
# Input: r3 = h, r4 = s, r5 = mac
# mac = h + s
#
.global Poly1305_emit_44
Poly1305_emit_44:
	SAVE_REGS

	# load 44 bits masks
	ld	10, rmask@got(2)
	li	6, 16
	li	7, 32
	lxvb16x 32+14, 6, 10	# 0xfffffffffff
	lxvb16x 32+15, 7, 10	# 0x3ffffffffff

	ld	10, cnum@got(2)
	lxv	32+16, 0(10)	# 0x2c - 44
	lxv	32+17, 16(10)	# 0x2a - 42

	ld	10, 0(3)
	ld	11, 8(3)
	ld	12, 16(3)

        mtvsrdd 32+0, 0, 10
        mtvsrdd 32+1, 0, 11
        mtvsrdd 32+2, 0, 12

	bl	Carry_reduction

	mfvsrld	10, 32+0
	mfvsrld	11, 32+1
	mfvsrld	12, 32+2

	# convert to 64 bits
	sldi	6, 11, 44
	or	10, 10, 6	# h0
	srdi	11, 11, 20	# 24 bits
	sldi	7, 12, 24
	or	11, 11, 7	# h1
	srdi	12, 12, 40	# h2

	# compare modulus
	mr	6, 10
	mr	7, 11
	mr	8, 12
	addic.	6, 6, 5
	addze	7, 7
	addze	8, 8
	srdi	9, 8, 2		# overflow?
	cmpdi	9, 0
	beq	Skip_h44
	mr	10, 6
	mr	11, 7
	mr	12, 8
Skip_h44:
	ld	6, 0(4)
	ld	7, 8(4)
	addc	10, 10, 6
	adde	11, 11, 7
	addze	12, 12

	std	10, 0(5)
	std	11, 8(5)
	#std	12, 16(5)

	RESTORE_REGS

	blr

.data
.align 5
rmask:
.byte   0xff, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x0f
.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff
.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff
mask44:
.long	0xffffffff, 0x00003ff, 0xffffffff, 0x00000fff
cnum:
.long   0x2c, 0x00, 0x2c, 0x00
.long   0x2a, 0x00, 0x2a, 0x00
