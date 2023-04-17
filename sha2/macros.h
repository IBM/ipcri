// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
#if defined(BIG) && defined(LITTLE)
#error "Both BIG and LITTLE defined, pick one"
#elif !defined(BIG) && !defined(LITTLE)
#error "Neither BIG or LITTLE are defined"
#endif

// Convert a regular VR to a VSR
// define(`VSR', `eval($1 + 32)')
#define VSR(r) (r+32)

.macro LOAD_FROM_DATA tgt, symb, tmp
	addis	\tmp,2,\symb@got@ha
	ld	\tmp,\symb@got@l(\tmp)
	lvx	\tgt,0,\tmp
.endm

.macro STARTFUNC func
.globl \func
.type \func,%function
\func:
.endm
