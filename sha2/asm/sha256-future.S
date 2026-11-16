// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright IBM Corp. 2026
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
.file "sha256-pfuture.S"

#include "macros.h"
#include "future.h"

// Register usage:

#define SP        1
#define TOCP      2
#define STATE     3
#define K         4
#define NUMBLOCKS 5
#define INPUT     6
#define T0        7
#define TC0       8
#define TC16      9

#define V0        0
#define V1        1
#define V2        2
#define V3        3
#define V4        4
#define V5        5
#define V6        6
#define V7        7
#define V8        8
#define V9        9
#define V10       10
#define V11       11
#define V12       12
#define V13       13
#define V14       14
#define V15       15

#define VEXPAB    16
#define VEXPCD    17
#define VPACK     18
#define VZERO     19

#define AT        1
#define AB        2


.text

	// xxinstdmr512 AT, XAp, XBp, i
	//  DMR[AT][4*i+0]  <= VSR[XA]
	//  DMR[AT][4*i+1]  <= VSR[XA+1]
	//  DMR[AT][4*i+2]  <= VSR[XB]
	//  DMR[AT][4*i+3]  <= VSR[XB+1]
	//  DMR[AT][rest]   <= DMR[AT][rest]



STARTFUNC ipcri_sha256_func
	cmpldi	0, NUMBLOCKS, 0
	ble		.done
	mtctr	NUMBLOCKS

	// Zero out DMR
	dmsetdmrz	AT
	dmsetdmrz	AB

	vxor	VZERO, VZERO, VZERO
	li	TC0, 0
	li	TC16, 16

	LOAD_FROM_DATA VEXPAB .load_maskAB T0
	LOAD_FROM_DATA VEXPCD .load_maskCD T0
	LOAD_FROM_DATA VPACK  .store_mask  T0

	// Step 1: Pack State into AT, zero expand 32b words to 64b words
	//  load from STATE
	lxvw4x	VSR(V4), STATE, TC0
	lxvw4x	VSR(V6), STATE, TC16

	// zero-expand and endian flip with vperm
	# V0 = 0, A, 0, B
	# V1 = 0, C, 0, D
	# V2 = 0, E, 0, F
	# V3 = 0, G, 0, H

	vperm	V0, V4, VZERO, VEXPAB
	vperm 	V1, V4, VZERO, VEXPCD
	vperm	V2, V6, VZERO, VEXPAB
	vperm	V3, V6, VZERO, VEXPCD

	dmxxinstdmr512	AT, VSR(V0), VSR(V2), 0

.loop:
	// Step 2: Pack Input into AB, zero expand 32b words to 64b words

	// AT = State and output (8 words)
	// AB = Input data		(16 words)

	lxvb16x	VSR(V8), INPUT, TC0
	addi	INPUT, INPUT, 16
	lxvb16x	VSR(V9), INPUT, TC0
	addi	INPUT, INPUT, 16
	lxvb16x	VSR(V10), INPUT, TC0
	addi	INPUT, INPUT, 16
	lxvb16x	VSR(V11), INPUT, TC0
	addi	INPUT, INPUT, 16

	vperm	V0, V8, VZERO, VEXPAB
	vperm	V1, V8, VZERO, VEXPCD
	vperm	V2, V9, VZERO, VEXPAB
	vperm	V3, V9, VZERO, VEXPCD
	vperm	V4, V10, VZERO, VEXPAB
	vperm	V5, V10, VZERO, VEXPCD
	vperm	V6, V11, VZERO, VEXPAB
	vperm	V7, V11, VZERO, VEXPCD

	dmxxinstdmr512	AB, VSR(V0), VSR(V2), 0
	dmxxinstdmr512	AB, VSR(V4), VSR(V6), 1

	// Step 3: Compress
	dmsha2hash	AT, AB, 0

	bdnz	.loop

	// Step 4: Unpack hash value from AT

	dmxxextfdmr512	VSR(V0), VSR(V2), AT, 0

	vperm	V4, V0, V1, VPACK
	vperm	V5, V2, V3, VPACK

	stxvw4x	VSR(V4), STATE, TC0
	stxvw4x	VSR(V5), STATE, TC16

.done:

	blr


.data
.align 4
.load_maskAB:
	.byte 17,17,17,17, 7,6,5,4, 17,17,17,17, 3,2,1,0
.align 4
.load_maskCD:
	.byte 17,17,17,17, 15,14,13,12, 17,17,17,17, 11,10,9,8
.align 4
.store_mask:
	.byte 27,26,25,24, 19,18,17,16, 11,10,9,8, 3,2,1,0
