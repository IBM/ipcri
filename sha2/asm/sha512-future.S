// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright IBM Corp. 2026
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
.file "sha512-pfuture.S"

#include "macros.h"
#include <future.h>

#define SP        1
#define TOCP      2
#define STATE     3
#define K         4
#define NUMBLOCKS 5
#define INPUT     6
#define T0        7
#define TC0       8
#define TC16      9
#define TC32      10
#define TC48      11

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


STARTFUNC ipcri_sha512_func
	cmpldi	0, NUMBLOCKS, 0
	ble		.done
	mtctr	NUMBLOCKS

	// Zero out DMRs
	dmsetdmrz	AT
	dmsetdmrz	AB

	li	TC0, 0
	li	TC16, 16
	li	TC32, 32
	li	TC48, 48

	// Step 1: Pack State into AT
	//  load from STATE

	lxvd2x	VSR(V0), STATE, TC0
	lxvd2x	VSR(V1), STATE, TC16
	lxvd2x	VSR(V2), STATE, TC32
	lxvd2x	VSR(V3), STATE, TC48

	dmxxinstdmr512	AT, VSR(V0), VSR(V2), 0

.loop:
	// Step 2: Pack Input into AB, loop n = NUMBLOCKS times

	// AT = State and output (8 words)
	// AB = Input data		(16 words)

	lxvb16x	VSR(V4), INPUT, TC0
	lxvb16x	VSR(V5), INPUT, TC16
	lxvb16x	VSR(V6), INPUT, TC32
	lxvb16x	VSR(V7), INPUT, TC48
	addi	INPUT, INPUT, 64
	lxvb16x	VSR(V8), INPUT, TC0
	lxvb16x	VSR(V9), INPUT, TC16
	lxvb16x	VSR(V10), INPUT, TC32
	lxvb16x	VSR(V11), INPUT, TC48
	addi	INPUT, INPUT, 64

	dmxxinstdmr512	AB, VSR(V4), VSR(V6), 0
	dmxxinstdmr512	AB, VSR(V8), VSR(V10), 1

	// Step 3: Compress
	dmsha2hash	AT, AB, 1

	bdnz	.loop

	// Step 4: Unpack hash value from AT

	dmxxextfdmr512	VSR(V0), VSR(V2), AT, 0

	stxvd2x	VSR(V0), STATE, TC0
	stxvd2x	VSR(V1), STATE, TC16
	stxvd2x	VSR(V2), STATE, TC32
	stxvd2x	VSR(V3), STATE, TC48

.done:

    blr
