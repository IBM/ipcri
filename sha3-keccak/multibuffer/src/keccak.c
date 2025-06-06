// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#include "keccak_mult.h"

static void keccak_reset0(keccak_ppc64_mult *kcx)
{
	memset(kcx, 0, sizeof(*kcx));
}

static void keccak_empty_state(keccak_ppc64_mult *kcx)
{
	memset(kcx->state, 0, sizeof(kcx->state));
}

void keccak_absorb_mult(keccak_ppc64_mult *kcx, int entrysz, u8 *puff1, int entrysz2, u8 *puff2)
{
	TRY_PASS(entrysz == entrysz2);
	MEMCHECK_VOID(puff1, NULL);
	MEMCHECK_VOID(puff2, NULL);

	u32 w, b;
	u32 blocks;
	u64 *block;
	u64 *block2;
	u32 total;
	u32 width;

	w = 0;
	b = 0;

	width = kcx->sponge._block_rate / WORDSZ;
	blocks = entrysz / _bytes(kcx->sponge._block_rate);
	total = blocks * width;

	while (total) {
		block = (u64 *) puff1 + b * width;
		block2 = (u64 *) puff2 + b * width;

		kcx->state[0][w] ^= block[w];
		kcx->state[1][w] ^= block2[w];
		w += 1;

		if (w == width) {
			keccakf_1600_mult(kcx->state[0], kcx->state[1]);
			total -= width;

			b += 1;
			w = 0;
		}
	}
}

void keccak_squeeze_mult(keccak_ppc64_mult *kcx, u8 *puff1, u8 *puff2)
{
	MEMCHECK_VOID(puff1, NULL);
	MEMCHECK_VOID(puff2, NULL);

	u32 off;
	u32 bsize;

	off = 0;
	while (kcx->sponge._digestsz) {
		bsize = get_min_bsize(kcx->sponge._digestsz, kcx->sponge._block_rate);

		memcpy(&puff1[off], kcx->state[0], _bytes(bsize));
		memcpy(&puff2[off], kcx->state[1], _bytes(bsize));

		off += _bytes(bsize);
		kcx->sponge._digestsz -= bsize;

		if (kcx->sponge._digestsz > 0) {
			keccakf_1600_mult(kcx->state[0], kcx->state[1]);
		}
	}
}

// returns the total number of characters with padding
u32 keccak_pad1_0_1_mult(keccak_ppc64_mult *kcx, int entrysz, u8 *entry, u8 *puff)
{
	MEMCHECK_INT(puff, NULL);

	u32 nchunks;
	u32 chnksz;
	u32 plen;

	nchunks = get_nchunks(entrysz, kcx->sponge._block_rate);
	chnksz = get_chunksz(nchunks, kcx->sponge._block_rate);
	plen = kcx->sponge._block_rate - (entrysz % kcx->sponge._block_rate);

	memset(puff, 0, chnksz * sizeof(u8));
	memcpy(puff, entry, entrysz);

	if (plen == 1) {
		puff[entrysz] = 0x86;
	} else {
		puff[entrysz] = kcx->sponge._pad;
		puff[chnksz - 1] |= KECCAK_DOM;
	}
	return chnksz;
}

void keccak_mult(keccak_ppc64_mult *kcx, int entrysz1, int entrysz2, u8 *entrybytes1, u8 *puffbytes1, u8 *entrybytes2, u8 *puffbytes2)
{
	TRY_PASS(kcx->sponge._block_rate <= 0 || (kcx->sponge._block_rate % 8 == 0));
	TRY_PASS(kcx->sponge._digestsz % 8 == 0);
	TRY_PASS((kcx->sponge._block_rate + kcx->sponge._capacity) == KECCAK_BITS_TOTAL);

	u32 pbuffsz;
	u8 *puff1;
	u8 *puff2;

	// padded buffer size calculation
	pbuffsz = 0;
	pbuffsz = get_padded_buffsz(kcx->sponge._block_rate, entrysz1);
	TRY_PASS(pbuffsz > 0);

	puff1 = NULL;
	puff1 = malloc(pbuffsz * sizeof(u8));
	MEMCHECK_VOID(puff1, NULL);

	puff2 = NULL;
	puff2 = malloc(pbuffsz * sizeof(u8));
	MEMCHECK_VOID(puff2, NULL);

	// padded 'message' size
	entrysz1 = keccak_pad1_0_1_mult(kcx, entrysz1, entrybytes1, puff1);
	entrysz2 = keccak_pad1_0_1_mult(kcx, entrysz2, entrybytes2, puff2);

	keccak_absorb_mult(kcx, entrysz1, puff1, entrysz2, puff2);
	keccak_squeeze_mult(kcx, puffbytes1, puffbytes2);

	free(puff1);
	free(puff2);
}

void sha3_call_mult(u32 bits, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2)
{
	keccak_ppc64_mult kcx = {
		.state[0] = { 0 },
		.state[1] = { 0 }
	};
	kcx.sponge._capacity = KECCAK_CAPACITY(bits);
	kcx.sponge._block_rate = KECCAK_RATE(bits);
	kcx.sponge._digestsz = bits;
	kcx.sponge._pad = SHA3_SUFFIX;
	keccak_mult(&kcx, entrysz1, entrysz2, entry1, puff1, entry2, puff2);
}

void shake128_call_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2)
{
	keccak_ppc64_mult kcx = {
		.state[0] = { 0 },
		.state[1] = { 0 }
	};
	kcx.sponge._capacity = KECCAK_CAPACITY(SHAKE128_DIGEST);
	kcx.sponge._block_rate = KECCAK_RATE(SHAKE128_DIGEST);
	kcx.sponge._digestsz = (puffsz) * 2;
	kcx.sponge._pad = SHAKE_SUFFIX;
	keccak_mult(&kcx, entrysz1, entrysz2, entry1, puff1, entry2, puff2);
}

void shake256_call_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2)
{
	keccak_ppc64_mult kcx = {
		.state[0] = { 0 },
		.state[1] = { 0 }
	};
	kcx.sponge._capacity = KECCAK_CAPACITY(SHAKE256_DIGEST);
	kcx.sponge._block_rate = KECCAK_RATE(SHAKE256_DIGEST);
	kcx.sponge._digestsz = (puffsz) * 2;
	kcx.sponge._pad = SHAKE_SUFFIX;
	keccak_mult(&kcx, entrysz1, entrysz2, entry1, puff1, entry2, puff2);
}

/* Progressive Shakes */

void keccak_shake_absorb_mult(keccak_ppc64_mult *kcx, uint8_t *entry1, uint8_t *entry2, u32 entrysz)
{
	TRY_PASS(kcx->sponge._block_rate <= 0 || (kcx->sponge._block_rate % 8 == 0));

	u64 i;
	u64 end;
	u8 *st1, *st2;
	u64 *stptr1, *stptr2;
	u64 nwords;
	u32 cont;

	end = kcx->sponge._block_rate;
	st1 = (u8 *) kcx->state[0];
	st2 = (u8 *) kcx->state[1];

	while (entrysz > 0) {
		i = 0;
		u64 untouched = get_min_bsize((kcx->sponge._block_rate - kcx->sponge._threshold), entrysz);
		nwords = untouched / 8;

		u64 *vstate1 = (u64 *) &st1[kcx->sponge._threshold];
		u64 *vstate2 = (u64 *) &st2[kcx->sponge._threshold];

		for (; i < nwords; i++) {
			stptr1 = &vstate1[i];
			*stptr1 ^= *((u64 *) &entry1[i * 8]);

			stptr2 = &vstate2[i];
			*stptr2 ^= *((u64 *) &entry2[i * 8]);
		}

		cont = nwords * 8;
		for (i = cont; i < untouched; i++) {
			((u8 *) vstate1)[i] ^= entry1[i];
			((u8 *) vstate2)[i] ^= entry2[i];
		}

		kcx->sponge._threshold += untouched;
		entry1 += untouched;
		entry2 += untouched;
		entrysz -= untouched;

		if (kcx->sponge._threshold == end) {
			keccakf_1600_mult(kcx->state[0], kcx->state[1]);
			kcx->sponge._threshold = 0;
		}
	}
}

void keccak_shake_complete_absorb_mult(keccak_ppc64_mult *kcx)
{
	uint8_t *state;
	uint8_t *state1;

	state = U8_ST(kcx->state[0]);
	state1 = U8_ST(kcx->state[1]);

	if (kcx->sponge._threshold != kcx->sponge._block_rate) {
		PAD_ST(kcx, state);
		PAD_ST(kcx, state1);
	}

	keccakf_1600_mult(kcx->state[0], kcx->state[1]);
	kcx->sponge._threshold = 0;
}

void keccak_shake_squeeze_mult(keccak_ppc64_mult *kcx, uint8_t *puff1, uint8_t *puff2, u64 puffsz)
{
	MEMCHECK_VOID(puff1, NULL);
	MEMCHECK_VOID(puff2, NULL);

	u64 i = 0;
	u64 block;
	u64 offset;
	u8 *st0;
	u8 *st1;

	st0 = U8_ST(kcx->state[0]);
	st1 = U8_ST(kcx->state[1]);

	while (puffsz > 0) {
		block = get_min_bsize(puffsz, kcx->sponge._avail);
		offset = kcx->sponge._block_rate - kcx->sponge._avail;

		memcpy(puff1 + i, st0 + offset, block);
		memcpy(puff2 + i, st1 + offset, block);

		i += block;
		puffsz -= block;
		kcx->sponge._avail -= block;

		if (puffsz) {
			keccakf_1600_mult(kcx->state[0], kcx->state[1]);
			if (!(kcx->sponge._avail)) {
				kcx->sponge._avail = kcx->sponge._block_rate;
			}
		}
	}
}

void keccak_shake_mult(keccak_ppc64_mult *kcx, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2, u32 puffsz)
{
	TRY_PASS(entrysz1 == entrysz2);
	keccak_shake_absorb_mult(kcx, entry1, entry2, entrysz1);
	keccak_shake_complete_absorb_mult(kcx);
	keccak_shake_squeeze_mult(kcx, puff1, puff2, puffsz);
	keccak_empty_state(kcx);
}

/* SHAKE128 */
void shake128_call_configure_mult(keccak_ppc64_mult *kcx)
{
	keccak_reset0(kcx);
	kcx->sponge._block_rate = SHAKE128_RATE;
	kcx->sponge._pad = SHAKE_SUFFIX;
	kcx->sponge._capacity = KECCAK_CAPACITY(SHAKE128_DIGEST);
	kcx->sponge._avail = kcx->sponge._block_rate;
}

void shake128_call_progressive_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2)
{
	keccak_ppc64_mult kcx;
	shake128_call_configure_mult(&kcx);
	keccak_shake_mult(&kcx, entry1, entry2, entrysz1, entrysz2, puff1, puff2, puffsz);
}

/* SHAKE256 */
void shake256_call_configure_mult(keccak_ppc64_mult *kcx)
{
	keccak_reset0(kcx);
	kcx->sponge._block_rate = SHAKE256_RATE;
	kcx->sponge._pad = SHAKE_SUFFIX;
	kcx->sponge._capacity = KECCAK_CAPACITY(SHAKE256_DIGEST);
	kcx->sponge._avail = kcx->sponge._block_rate;
}

void shake256_call_progressive_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2)
{
	keccak_ppc64_mult kcx;
	shake256_call_configure_mult(&kcx);
	keccak_shake_mult(&kcx, entry1, entry2, entrysz1, entrysz2, puff1, puff2, puffsz);
}
