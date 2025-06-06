// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#include "keccak.h"

void keccak_reset0(keccak_ppc64_t *kcx)
{
	memset(kcx, 0, sizeof(*kcx));
}

void keccak_empty_state(keccak_ppc64_t *kcx)
{
	memset(&kcx->state, 0, STATE_SIZE * sizeof(u64));
}

void keccak_absorb(keccak_ppc64_t *kcx, int entrysz, u8 *puff)
{
	MEMCHECK_VOID(puff, NULL);

	u32 w, b;
	u64 *segment;
	u64 *puffu64;
	u32 span;
	u32 sponge_block_rate;
	u32 width;

	w = 0;
	b = 0;

	sponge_block_rate = kcx->sponge._block_rate;
	width = sponge_block_rate / WORDSZ;
	span = (entrysz / _bytes(sponge_block_rate)) * width;
	puffu64 = (u64 *) puff;

	while (span) {
		segment = puffu64 + b * width;

		kcx->state[w] ^= segment[w];
		w += 1;

		if (w == width) {
			keccakf_1600(kcx->state);
			span -= width;

			b += 1;
			w = 0;
		}
	}
}

void keccak_squeeze(keccak_ppc64_t *kcx, u8 *puff)
{
	MEMCHECK_VOID(puff, NULL);

	u32 off;
	u32 i;
	u32 bsize;
	u64 *tmpptr;
	u64 tmparr[STATE_SIZE];

	off = 0;
	while (kcx->sponge._digestsz) {
		bsize = get_min_bsize(kcx->sponge._digestsz, kcx->sponge._block_rate);

		for (i = 0; i < STATE_SIZE; i++) {
			tmpptr = (u64 *) &(kcx->state[i]);
			tmparr[i] = *tmpptr;
		}

		memcpy(&puff[off], tmparr, _bytes(bsize));
		off += _bytes(bsize);
		kcx->sponge._digestsz -= bsize;

		if (kcx->sponge._digestsz) {
			keccakf_1600(kcx->state);
		}
	}
}

// returns the total number of characters with padding
u32 keccak_pad1_0_1(keccak_ppc64_t *kcx, int entrysz, u8 *entry, u8 *puff)
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
		entry[entrysz] = 0x86;
	} else {
		puff[entrysz] = kcx->sponge._pad;
		puff[chnksz - 1] |= KECCAK_DOM;
	}
	return chnksz;
}

void keccak(keccak_ppc64_t *kcx, int entrysz, u8 *entrybytes, u8 *puff)
{
	MEMCHECK_VOID(puff, NULL);
	TRY_PASS((kcx->sponge._block_rate <= 0) || (kcx->sponge._block_rate % 8 == 0));
	TRY_PASS((kcx->sponge._block_rate + kcx->sponge._capacity) == KECCAK_BITS_TOTAL);

	u32 pbuffsz;
	u8 *buf;

	// padded buffer size calculation
	pbuffsz = 0;
	pbuffsz = get_padded_buffsz(kcx->sponge._block_rate, entrysz);
	TRY_PASS(pbuffsz > 0);

	buf = NULL;
	buf = malloc(pbuffsz * sizeof(u8));
	MEMCHECK_VOID(buf, NULL);

	// padded 'message' size
	entrysz = keccak_pad1_0_1(kcx, entrysz, entrybytes, buf);

	keccak_absorb(kcx, entrysz, buf);
	keccak_squeeze(kcx, puff);
	keccak_empty_state(kcx);

	free(buf);
}

void shake128_call(u32 puffsz, u8 *entry, int entrysz, u8 *puff)
{
	keccak_ppc64_t kcx;
	keccak_reset0(&kcx);
	kcx.sponge._capacity = KECCAK_CAPACITY(SHAKE128_DIGEST);
	kcx.sponge._block_rate = KECCAK_RATE(SHAKE128_DIGEST);
	kcx.sponge._digestsz = (puffsz) * 2;
	kcx.sponge._pad = SHAKE_SUFFIX;
	keccak(&kcx, entrysz, entry, puff);
}

void shake256_call(u32 puffsz, u8 *entry, int entrysz, u8 *puff)
{
	keccak_ppc64_t kcx;
	keccak_reset0(&kcx);
	kcx.sponge._capacity = KECCAK_CAPACITY(SHAKE256_DIGEST);
	kcx.sponge._block_rate = KECCAK_RATE(SHAKE256_DIGEST);
	kcx.sponge._digestsz = (puffsz) * 2;
	kcx.sponge._pad = SHAKE_SUFFIX;
	keccak(&kcx, entrysz, entry, puff);
}

void sha3_call(u32 bits, u8 *entry, int entrysz, u8 *puff)
{
	keccak_ppc64_t kcx;
	keccak_reset0(&kcx);
	kcx.sponge._capacity = KECCAK_CAPACITY(bits);
	kcx.sponge._block_rate = KECCAK_RATE(bits);
	kcx.sponge._digestsz = bits;
	kcx.sponge._pad = SHA3_SUFFIX;
	keccak(&kcx, entrysz, entry, puff);
}

/*Progressive Shakes*/

void xoru64(u64 nwords, u64 *vstate, u8 *entry)
{
	u64 *stptr;
	u32 i;
	for (i = 0; i < nwords; i++) {
		stptr = &vstate[i];
		*stptr ^= *((u64 *) &entry[i * 8]);
	}
}

void xoru8(u64 nwords, u64 *vstate, u8 *entry, u64 untouched)
{
	u32 i;
	u32 cont;
	cont = nwords * 8;
	for (i = cont; i < untouched; i++) {
		((u8 *) vstate)[i] ^= entry[i];
	}
}

void keccak_shake_absorb(keccak_ppc64_t *kcx, u8 *entry, size_t entrysz)
{
	u64 end;
	u64 untouched;
	u8 *sponge;
	u64 nwords;

	end = kcx->sponge._block_rate;
	sponge = (u8 *) kcx->state;

	while (entrysz > 0) {
		untouched = get_min_bsize((kcx->sponge._block_rate - kcx->sponge._threshold), entrysz);
		nwords = untouched / 8;
		u64 *vstate = __builtin_assume_aligned((u64 *) (sponge + kcx->sponge._threshold), 8);

		xoru64(nwords, vstate, entry);
		xoru8(nwords, vstate, entry, untouched);

		kcx->sponge._threshold += untouched;
		entry += untouched;
		entrysz -= untouched;

		if (kcx->sponge._threshold == end) {
			keccakf_1600(kcx->state);
			kcx->sponge._threshold = 0;
		}
	}
}

void keccak_shake_complete_absorb(keccak_ppc64_t *kcx)
{
	u8 *state;
	state = U8_ST(kcx->state);

	if (kcx->sponge._threshold != kcx->sponge._block_rate) {
		PAD_ST(kcx, state);
	}

	keccakf_1600(kcx->state);
	kcx->sponge._threshold = 0;
}

void keccak_shake_squeeze(keccak_ppc64_t *kcx, uint8_t *puff, u64 puffsz)
{
	MEMCHECK_VOID(puff, NULL);
	u64 i = 0;
	u64 segment;
	u64 offset;
	u8 *st;

	st = U8_ST(kcx->state);

	while (puffsz > 0) {
		segment = get_min_bsize(puffsz, kcx->sponge._avail);
		offset = kcx->sponge._block_rate - kcx->sponge._avail;
		memcpy(puff + i, st + offset, segment);

		i += segment;
		puffsz -= segment;
		kcx->sponge._avail -= segment;

		if (puffsz) {
			keccakf_1600(kcx->state);
			if (!(kcx->sponge._avail)) {
				kcx->sponge._avail = kcx->sponge._block_rate;
			}
		}
	}
}

void shake_process(keccak_ppc64_t *kcx, uint8_t *entry, uint64_t entrysz, uint8_t *puff, uint64_t puffsz)
{
	keccak_shake_absorb(kcx, entry, entrysz);
	keccak_shake_complete_absorb(kcx);
	keccak_shake_squeeze(kcx, puff, puffsz);
	keccak_empty_state(kcx);
}

void shake128_call_configure(keccak_ppc64_t *kcx)
{
	keccak_reset0(kcx);
	kcx->sponge._block_rate = SHAKE128_RATE;
	kcx->sponge._pad = SHAKE_SUFFIX;
	kcx->sponge._capacity = KECCAK_CAPACITY(SHAKE128_DIGEST);
	kcx->sponge._avail = kcx->sponge._block_rate;
}

void shake128_call_progressive(u32 puffsz, u8 *entry, int entrysz, u8 *puff)
{
	keccak_ppc64_t kcx;
	shake128_call_configure(&kcx);
	shake_process(&kcx, entry, entrysz, puff, puffsz);
}

void shake256_call_configure(keccak_ppc64_t *kcx)
{
	keccak_reset0(kcx);
	kcx->sponge._block_rate = SHAKE256_RATE;
	kcx->sponge._pad = SHAKE_SUFFIX;
	kcx->sponge._capacity = KECCAK_CAPACITY(SHAKE256_DIGEST);
	kcx->sponge._avail = kcx->sponge._block_rate;
}

void shake256_call_progressive(u32 puffsz, u8 *entry, int entrysz, u8 *puff)
{
	keccak_ppc64_t kcx;
	shake256_call_configure(&kcx);
	shake_process(&kcx, entry, entrysz, puff, puffsz);
}
