// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
.file "sha512-compress.S"

#include "macros.h"

// Parameters in
#define SP 1
#define STATE 3
#define K 4
#define NUMBLOCKS 5
#define INPUT 6

#define T0 7
#define T1 8
#define TK 9
#define COUNT 10

// State registers
#define VSA 0
#define VSB 1
#define VSC 2
#define VSD 3
#define VSE 4
#define VSF 5
#define VSG 6
#define VSH 7

// Current K values
#define VK 8

// Temp registers for math
#define VT0 9
#define VT1 10
#define VT2 11
#define VT3 12
#define VT4 13

// Convenience named registers for sigma(a) and sigma(e)
#define SIGA 14
#define SIGE 15

// Input words W[i]. Not directly referenced, but defined here to keep track
#define VW0 16
#define VW1 17
#define VW2 18
#define VW3 19
#define VW4 20
#define VW5 21
#define VW6 22
#define VW7 23
#define VW8 24
#define VW9 25
#define VW10 26
#define VW11 27
#define VW12 28
#define VW13 29
#define VW14 30
#define VW15 31

// Convert an index for W[i] to the corresponding register
#define IV(r) (r+VW0)

.macro ROUND A B C D E F G H R EXT

	vaddudm	VT1, VK, IV(\R)	// VT1: k+W
	vaddudm	VT4, \H, VT1	// VT4: H+k+W

	lxvd2x	VSR(VK), TK, K	// Load Key
	addi	TK, TK, 8		// Increment Pointer to next key

	vaddudm	VT2, \D, \H		// VT2: H+D
	vaddudm	VT2, VT2, VT1	// VT2: H+D+k+W

	vshasigmad	SIGE, \E, 1, 0b1111	// Sigma(E)  Se
	vshasigmad	SIGA, \A, 1, 0		// Sigma(A)  Sa

	vxor	VT3, \B, \C			// VT3: b^c
	vsel	VT0, \G, \F, \E		// VT0: Ch.
	vsel	VT3, \C, \A, VT3	// VT3: Maj(a,b,c)

	vaddudm	VT4, VT4, VT0	// VT4: Hkw + Ch.
	vaddudm	VT3, VT3, VT4	// VT3: HkW + Ch. + Maj.

	vaddudm	VT0, VT0, VT2	// VT0: Ch. + DHKW
	vaddudm	\H, SIGE, SIGA	// Anext: Se + Sa
	vaddudm	\D, VT0, SIGE	// Dnext: Ch. + DHKW + Se
	vaddudm	\H, \H, VT3		// Anext: Se+Sa+HkW+Ch.+Maj.


	// Schedule (data) for 16th round in future
	// Extend W[i]
	.if \EXT
		vshasigmad	SIGE, IV((\R + 14) % 16), 0, 0b1111
		vshasigmad	SIGA, IV((\R + 1) % 16), 0, 0b0000
		vaddudm		IV(\R), IV(\R), SIGE
		vaddudm		IV(\R), IV(\R), SIGA
		vaddudm		IV(\R), IV(\R), IV((\R + 9) % 16)
	.endif
.endm

#define EXTENDROUND(a,b,c,d,e,f,g,h,r)	ROUND a b c d e f g h r 1
#define NOEXTENDROUND(a,b,c,d,e,f,g,h,r)	ROUND a b c d e f g h r 0

.macro NOEXTENDROUNDS
	NOEXTENDROUND(VSA, VSB, VSC, VSD, VSE, VSF, VSG, VSH, 0)
	NOEXTENDROUND(VSH, VSA, VSB, VSC, VSD, VSE, VSF, VSG, 1)
	NOEXTENDROUND(VSG, VSH, VSA, VSB, VSC, VSD, VSE, VSF, 2)
	NOEXTENDROUND(VSF, VSG, VSH, VSA, VSB, VSC, VSD, VSE, 3)

	NOEXTENDROUND(VSE, VSF, VSG, VSH, VSA, VSB, VSC, VSD, 4)
	NOEXTENDROUND(VSD, VSE, VSF, VSG, VSH, VSA, VSB, VSC, 5)
	NOEXTENDROUND(VSC, VSD, VSE, VSF, VSG, VSH, VSA, VSB, 6)
	NOEXTENDROUND(VSB, VSC, VSD, VSE, VSF, VSG, VSH, VSA, 7)

	NOEXTENDROUND(VSA, VSB, VSC, VSD, VSE, VSF, VSG, VSH, 8)
	NOEXTENDROUND(VSH, VSA, VSB, VSC, VSD, VSE, VSF, VSG, 9)
	NOEXTENDROUND(VSG, VSH, VSA, VSB, VSC, VSD, VSE, VSF, 10)
	NOEXTENDROUND(VSF, VSG, VSH, VSA, VSB, VSC, VSD, VSE, 11)

	NOEXTENDROUND(VSE, VSF, VSG, VSH, VSA, VSB, VSC, VSD, 12)
	NOEXTENDROUND(VSD, VSE, VSF, VSG, VSH, VSA, VSB, VSC, 13)
	NOEXTENDROUND(VSC, VSD, VSE, VSF, VSG, VSH, VSA, VSB, 14)
	NOEXTENDROUND(VSB, VSC, VSD, VSE, VSF, VSG, VSH, VSA, 15)
.endm

.macro EXTENDROUNDS
	EXTENDROUND(VSA, VSB, VSC, VSD, VSE, VSF, VSG, VSH, 0)
	EXTENDROUND(VSH, VSA, VSB, VSC, VSD, VSE, VSF, VSG, 1)
	EXTENDROUND(VSG, VSH, VSA, VSB, VSC, VSD, VSE, VSF, 2)
	EXTENDROUND(VSF, VSG, VSH, VSA, VSB, VSC, VSD, VSE, 3)

	EXTENDROUND(VSE, VSF, VSG, VSH, VSA, VSB, VSC, VSD, 4)
	EXTENDROUND(VSD, VSE, VSF, VSG, VSH, VSA, VSB, VSC, 5)
	EXTENDROUND(VSC, VSD, VSE, VSF, VSG, VSH, VSA, VSB, 6)
	EXTENDROUND(VSB, VSC, VSD, VSE, VSF, VSG, VSH, VSA, 7)

	EXTENDROUND(VSA, VSB, VSC, VSD, VSE, VSF, VSG, VSH, 8)
	EXTENDROUND(VSH, VSA, VSB, VSC, VSD, VSE, VSF, VSG, 9)
	EXTENDROUND(VSG, VSH, VSA, VSB, VSC, VSD, VSE, VSF, 10)
	EXTENDROUND(VSF, VSG, VSH, VSA, VSB, VSC, VSD, VSE, 11)

	EXTENDROUND(VSE, VSF, VSG, VSH, VSA, VSB, VSC, VSD, 12)
	EXTENDROUND(VSD, VSE, VSF, VSG, VSH, VSA, VSB, VSC, 13)
	EXTENDROUND(VSC, VSD, VSE, VSF, VSG, VSH, VSA, VSB, 14)
	EXTENDROUND(VSB, VSC, VSD, VSE, VSF, VSG, VSH, VSA, 15)
.endm

.macro LOAD w
	#ifdef BIG
		lxvd2x	VSR(IV(\w)), 0, INPUT
	#endif
	#ifdef LITTLE
		lxvd2x	VSR(IV(\w)), 0, INPUT
		vperm	IV(\w), IV(\w), IV(\w), VT0
	#endif
	addi	INPUT, INPUT, 8
.endm

.macro DOLOADS
	#ifdef LITTLE
	LOAD_FROM_DATA VT0 .load_swap T1
	#endif
	LOAD 0
	LOAD 1
	LOAD 2
	LOAD 3

	LOAD 4
	LOAD 5
	LOAD 6
	LOAD 7

	LOAD 8
	LOAD 9
	LOAD 10
	LOAD 11

	LOAD 12
	LOAD 13
	LOAD 14
	LOAD 15
.endm

.text
STARTFUNC ipcri_sha512_func
	cmpwi	0, NUMBLOCKS, 0
	ble	0, .done
	mtctr	NUMBLOCKS

	// Store non-volatile registers
	subi	SP, SP, 64+(12*16)
	std	T0,	24(SP)
	std	T1,	16(SP)
	std	COUNT,	8(SP)

	li	T0, 32
	stvx	20, 0, SP
	subi	T0, T0, 16
	stvx	21, T0, SP
	subi	T0, T0, 16
	stvx	22, T0, SP
	subi	T0, T0, 16
	stvx	23, T0, SP
	subi	T0, T0, 16
	stvx	24, T0, SP
	subi	T0, T0, 16
	stvx	25, T0, SP
	subi	T0, T0, 16
	stvx	26, T0, SP
	subi	T0, T0, 16
	stvx	27, T0, SP
	subi	T0, T0, 16
	stvx	28, T0, SP
	subi	T0, T0, 16
	stvx	29, T0, SP
	subi	T0, T0, 16
	stvx	30, T0, SP
	subi	T0, T0, 16
	stvx	31, T0, SP

	// Load state values
	li	T0, 16
	lxvd2x	VSR(VSA), 0, STATE	// VSA contains A, B
	lxvd2x	VSR(VSC), T0, STATE // VSC contains C, D
	addi	T0, T0, 16
	lxvd2x	VSR(VSE), T0, STATE	// VSE contains E, F
	addi	T0, T0, 16
	lxvd2x	VSR(VSG), T0, STATE	// VSG contains G, H


.loop:
	li	TK, 0
	lxvd2x	VSR(VK), TK, K
	addi	TK, TK, 8 // might need to be moved, or use swizzle

	DOLOADS

	// "permute" state from VSA containing A,B,C,D into VSA,VSB,VSC,VSD
	vsldoi	VSB, VSA, VSA, 8
	vsldoi	VSD, VSC, VSC, 8
	vsldoi	VSF, VSE, VSE, 8
	vsldoi	VSH, VSG, VSG, 8

	EXTENDROUNDS
	EXTENDROUNDS
	EXTENDROUNDS
	EXTENDROUNDS
	NOEXTENDROUNDS

	LOAD_FROM_DATA VT4 .pack_lr T0

	// Repack VSA,VSB,VSC,VSD into VSA for storing
	// Reload from stack
	li	T0, 16
	lxvd2x	VSR(VT0), 0, STATE
	lxvd2x	VSR(VT1), T0, STATE
	addi	T0, T0, 16
	lxvd2x	VSR(VT2), T0, STATE
	addi	T0, T0, 16
	lxvd2x	VSR(VT3), T0, STATE

	vperm	VSA, VSA, VSB, VT4
	vperm	VSC, VSC, VSD, VT4
	vperm	VSE, VSE, VSF, VT4
	vperm	VSG, VSG, VSH, VT4

	vaddudm	VSA, VSA, VT0
	vaddudm	VSC, VSC, VT1
	vaddudm	VSE, VSE, VT2
	vaddudm	VSG, VSG, VT3

	li	T0, 16
	stxvd2x	VSR(VSA), 0, STATE
	stxvd2x	VSR(VSC), T0, STATE
	addi	T0, T0, 16
	stxvd2x	VSR(VSE), T0, STATE
	addi	T0, T0, 16
	stxvd2x	VSR(VSG), T0, STATE

	bdnz	.loop

	// Restore nonvolatile registers
	li	T0, 32
	lvx	20, 0, SP
	subi	T0, T0, 16
	lvx	21, T0, SP
	subi	T0, T0, 16
	lvx	22, T0, SP
	subi	T0, T0, 16
	lvx	23, T0, SP
	subi	T0, T0, 16
	lvx	24, T0, SP
	subi	T0, T0, 16
	lvx	25, T0, SP
	subi	T0, T0, 16
	lvx	26, T0, SP
	subi	T0, T0, 16
	lvx	27, T0, SP
	subi	T0, T0, 16
	lvx	28, T0, SP
	subi	T0, T0, 16
	lvx	29, T0, SP
	subi	T0, T0, 16
	lvx	30, T0, SP
	subi	T0, T0, 16
	lvx	31, T0, SP

	ld	T0,	24(SP)
	ld	T1,	16(SP)
	ld	COUNT,	8(SP)
	addi	SP, SP, 64+(12*16)

.done:
	mr 3, INPUT

	blr

#ifdef LITTLE
.data
.align 4
.load_swap:
	.byte 8,9,10,11, 12,13,14,15, 0,1,2,3, 4,5,6,7
#endif
.align 4
.pack_lr:
	#if  defined(BIG)
	.byte 0,1,2,3,4,5,6,7, 16,17,18,19,20,21,22,23
	#elif defined(LITTLE)
	.byte 23,22,21,20,19,18,17,16, 7,6,5,4,3,2,1,0
	#endif
