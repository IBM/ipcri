// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

/* **************************************************************************************************************
 *
 * This code is based on the official documentation for https://csrc.nist.gov/csrc/media/Projects/hash-functions/documents/Keccak-reference-3.0.pdf
 * Follows the guidelines provided by https://keccak.team
 * Original keccak implementation from the keccak team: https://github.com/XKCP/XKCP
 *
 * additional helper sources:
 * https://csrc.nist.gov/csrc/media/projects/cryptographic-standards-and-guidelines/documents/examples/sha_all.pdf
 *
 ************************************************************************************************************** */

#ifndef KECCAK_B_H
#define KECCAK_B_H

#include "util.h"

/* prevent possible conflicts */
#if defined(__ALTIVEC__)
#include <altivec.h>
#undef vector
#undef pixel
#undef bool
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define V_L 0
#define V_H 1
#else
#define V_L 1
#define V_H 0
#endif

// ALL LENGTHS IN BYTES
#define SLICE 5 // the dimensions of the internal state
#define LANE 8
#define KECCAKl 6
#define ROUNDS (2 * KECCAKl) + 12 // 24
#define STATE_SIZE 5 * 5	  // State is an unrolled 5x5
#define KECCAK_BITS_TOTAL 25 << KECCAKl

#define WORDSZ 64
#define _byte 8
#define _bits(x) ((x) * _byte)
#define _bytes(x) ((x) / _byte)

#define get_nchunks(msz, r) (msz / _bytes(r))
#define get_chunksz(b, r) ((b + 1) * (_bytes(r)))
#define get_padded_buffsz(r, msz) (_bytes(r) * ((msz / _bytes(r)) + 1))
#define get_min_bsize(x, y) (((x) < (y)) ? (x) : (y))

#define SHA3_SUFFIX 0x06
#define SHAKE_SUFFIX 0x1F
#define KECCAK_DOM 0x80

#define SHA3_224_DIGEST 256
#define SHA3_256_DIGEST 256
#define SHA3_384_DIGEST 384
#define SHA3_512_DIGEST 512
#define SHAKE128_DIGEST 128
#define SHAKE256_DIGEST 256

typedef struct {
	u32 _block_rate;
	u32 _capacity;
	u32 _digestsz;
	u8 _pad;
	u64 _avail;
	u64 _threshold;
} sponge_components;

static const v2_u64 CROUND_TABLE[ROUNDS] = {
	{ 0x0000000000000001ULL, 0x0000000000000001ULL },
	{ 0x0000000000008082ULL, 0x0000000000008082ULL },
	{ 0x800000000000808aULL, 0x800000000000808aULL },
	{ 0x8000000080008000ULL, 0x8000000080008000ULL },
	{ 0x000000000000808bULL, 0x000000000000808bULL },
	{ 0x0000000080000001ULL, 0x0000000080000001ULL },
	{ 0x8000000080008081ULL, 0x8000000080008081ULL },
	{ 0x8000000000008009ULL, 0x8000000000008009ULL },
	{ 0x000000000000008aULL, 0x000000000000008aULL },
	{ 0x0000000000000088ULL, 0x0000000000000088ULL },
	{ 0x0000000080008009ULL, 0x0000000080008009ULL },
	{ 0x000000008000000aULL, 0x000000008000000aULL },
	{ 0x000000008000808bULL, 0x000000008000808bULL },
	{ 0x800000000000008bULL, 0x800000000000008bULL },
	{ 0x8000000000008089ULL, 0x8000000000008089ULL },
	{ 0x8000000000008003ULL, 0x8000000000008003ULL },
	{ 0x8000000000008002ULL, 0x8000000000008002ULL },
	{ 0x8000000000000080ULL, 0x8000000000000080ULL },
	{ 0x000000000000800aULL, 0x000000000000800aULL },
	{ 0x800000008000000aULL, 0x800000008000000aULL },
	{ 0x8000000080008081ULL, 0x8000000080008081ULL },
	{ 0x8000000000008080ULL, 0x8000000000008080ULL },
	{ 0x0000000080000001ULL, 0x0000000080000001ULL },
	{ 0x8000000080008008ULL, 0x8000000080008008ULL }
};

#define KECCAK_CAPACITY(n) ((n) * 2) // c = b - r (a.k.a (double the digest length))
#define KECCAK_RATE(n) (u32)((KECCAK_BITS_TOTAL) - KECCAK_CAPACITY(n))
#define KECCAK_BITS_TO_BYTES(n) (n / 8)

// sponge_block_rate returns the number of bytes of the internal state which can be absorbed
#define SHAKE128_RATE (KECCAK_BITS_TO_BYTES(KECCAK_RATE((SHAKE128_DIGEST))))
#define SHAKE256_RATE (KECCAK_BITS_TO_BYTES(KECCAK_RATE((SHAKE256_DIGEST))))
#define SHA3_224_RATE (KECCAK_BITS_TO_BYTES(KECCAK_RATE((SHA3_224_DIGEST))))
#define SHA3_256_RATE (KECCAK_BITS_TO_BYTES(KECCAK_RATE((SHA3_256_DIGEST))))
#define SHA3_384_RATE (KECCAK_BITS_TO_BYTES(KECCAK_RATE((SHA3_384_DIGEST))))
#define SHA3_512_RATE (KECCAK_BITS_TO_BYTES(KECCAK_RATE((SHA3_512_DIGEST))))

#endif
