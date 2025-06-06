// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#ifndef KECCAK_H
#define KECCAK_H

#include "../../common/keccak_base.h"

/* prevent possible conflicts */
#if defined(__ALTIVEC__)
#include <altivec.h>
#undef vector
#undef pixel
#undef bool
#endif

typedef struct {
	u64 state[STATE_SIZE];
	sponge_components sponge;
} keccak_ppc64_t;

void keccak_reset0(keccak_ppc64_t *kcx);
void keccak_empty_state(keccak_ppc64_t *kcx);

u32 keccak_pad1_0_1(keccak_ppc64_t *kcx, int entrysz, u8 *entry, u8 *puff);
void sha3_call(u32 bits, u8 *entry, int entrysz, u8 *puff);
void keccak(keccak_ppc64_t *kcx, int entrysz, u8 *entrybytes, u8 *puffbytes);
void keccak_absorb(keccak_ppc64_t *kcx, int entrysz, u8 *puff);
void keccak_squeeze(keccak_ppc64_t *kcx, uint8_t *puff);

void shake128_call(u32 bits, u8 *entry, int entrysz, u8 *puff);
void shake256_call(u32 bits, u8 *entry, int entrysz, u8 *puff);
void shake128_call_progressive(u32 bits, u8 *entry, int entrysz, u8 *puff);
void shake256_call_progressive(u32 bits, u8 *entry, int entrysz, u8 *puff);

void shake256_call_configure(keccak_ppc64_t *kcx);
void shake128_call_configure(keccak_ppc64_t *kcx);

void xoru8(u64 nwords, u64 *vstate, u8 *entry, u64 untouched);
void xoru64(u64 nwords, u64 *vstate, u8 *entry);

void shake_process(keccak_ppc64_t *kcx, uint8_t *buf, uint64_t buffsz, uint8_t *puff, uint64_t puffsz);
void keccak_shake_complete_absorb(keccak_ppc64_t *kcx);
void keccak_shake_absorb(keccak_ppc64_t *kcx, uint8_t *buf, uint64_t buffsz);
void keccak_shake_squeeze(keccak_ppc64_t *kcx, uint8_t *puff, u64 puffsz);

#endif
