// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
.file "sha256-compress.S"

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

	vadduwm	VT1, VK, IV(\R)	// VT1: k+W
	vadduwm	VT4, \H, VT1	// VT4: H+k+W

	lxvw4x	VSR(VK), TK, K	// Load Key
	addi	TK, TK, 4		// Increment Pointer to next key

	vadduwm	VT2, \D, \H		// VT2: H+D
	vadduwm	VT2, VT2, VT1	// VT2: H+D+k+W

	vshasigmaw	SIGE, \E, 1, 0b1111	// Sigma(E)  Se
	vshasigmaw	SIGA, \A, 1, 0		// Sigma(A)  Sa

	vxor	VT3, \B, \C			// VT3: b^c
	vsel	VT0, \G, \F, \E		// VT0: Ch.
	vsel	VT3, \C, \A, VT3	// VT3: Maj(a,b,c)

	vadduwm	VT4, VT4, VT0	// VT4: Hkw + Ch.
	vadduwm	VT3, VT3, VT4	// VT3: HkW + Ch. + Maj.

	vadduwm	VT0, VT0, VT2	// VT0: Ch. + DHKW
	vadduwm	\H, SIGE, SIGA	// Anext: Se + Sa
	vadduwm	\D, VT0, SIGE	// Dnext: Ch. + DHKW + Se
	vadduwm	\H, \H, VT3		// Anext: Se+Sa+HkW+Ch.+Maj.


	// Schedule (data) for 16th round in future
	// Extend W[i]
	.if \EXT
		vshasigmaw	SIGE, IV((\R + 14) % 16), 0, 0b1111
		vshasigmaw	SIGA, IV((\R + 1) % 16), 0, 0b0000
		vadduwm		IV(\R), IV(\R), SIGE
		vadduwm		IV(\R), IV(\R), SIGA
		vadduwm		IV(\R), IV(\R), IV((\R + 9) % 16)
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
		lxvw4x	VSR(IV(\w)), 0, INPUT
	#endif
	#ifdef LITTLE
		lxvd2x	VSR(IV(\w)), 0, INPUT
		vperm	IV(\w), IV(\w), IV(\w), VT0
	#endif
	addi	INPUT, INPUT, 4
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
STARTFUNC ipcri_sha256_func
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
	lxvw4x	VSR(VSA), 0, STATE	// VSA contains A,B,C,D
	lxvw4x	VSR(VSE), T0, STATE	// VSE contains E,F,G,H

.loop:
	li	TK, 0
	lxvw4x	VSR(VK), TK, K
	addi	TK, TK, 4 // might need to be moved, or use swizzle

	DOLOADS

	// "permute" state from VSA containing A,B,C,D into VSA,VSB,VSC,VSD
	vsldoi	VSB, VSA, VSA, 4
	vsldoi	VSF, VSE, VSE, 4

	vsldoi	VSC, VSA, VSA, 8
	vsldoi	VSG, VSE, VSE, 8

	vsldoi	VSD, VSA, VSA, 12
	vsldoi	VSH, VSE, VSE, 12

	EXTENDROUNDS
	EXTENDROUNDS
	EXTENDROUNDS
	NOEXTENDROUNDS

	// Repack VSA,VSB,VSC,VSD into VSA for storing
	// Reload from stack
	li	T0, 16
	lxvw4x	VSR(VT0), 0, STATE	// VSA contains A,B,C,D
	lxvw4x	VSR(VT1), T0, STATE	// VSE contains E,F,G,H

	vmrghw	VSA, VSA, VSB
	vmrghw	VSC, VSC, VSD
	vmrghw	VSE, VSE, VSF
	vmrghw	VSG, VSG, VSH

	xxmrghd	VSR(VSA), VSR(VSA), VSR(VSC)
	xxmrghd	VSR(VSE), VSR(VSE), VSR(VSG)

	vadduwm	VSA, VSA, VT0
	vadduwm	VSE, VSE, VT1

	li	T0, 16
	stxvw4x	VSR(VSA), 0, STATE
	stxvw4x	VSR(VSE), T0, STATE

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
