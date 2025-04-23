// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#ifndef KECCAK_MULT_H
#define KECCAK_MULT_H

#include "../../common/keccak_base.h"

/* prevent possible conflicts */
#if defined(__ALTIVEC__)
#include <altivec.h>
#undef vector
#undef pixel
#undef bool
#endif

typedef struct {
	u64 state[2][STATE_SIZE];
	sponge_components sponge;
} keccak_ppc64_mult;

u32 keccak_pad1_0_1_mult(keccak_ppc64_mult *kcx, int entrysz, u8 *entry, u8 *puff);
void sha3_call_mult(u32 bits, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2);
void shake128_call_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2);
void shake256_call_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2);

void keccak_absorb_mult(keccak_ppc64_mult *kcx, int entrysz, u8 *puff1, int entrysz2, u8 *puff2);
void keccak_squeeze_mult(keccak_ppc64_mult *kcx, u8 *puff1, u8 *puff2);
void keccak_mult(keccak_ppc64_mult *kcx, int entrysz1, int entrysz2, u8 *entrybytes1, u8 *puffbytes1, u8 *entrybytes2, u8 *puffbytes2);
void keccak_absorb_load_mult_states(keccak_ppc64_mult *kcx, u32 inx, u64 **stptr1, u64 **stptr2);

void keccak_shake_squeeze_mult(keccak_ppc64_mult *kcx, uint8_t *puff1, uint8_t *puff2, u64 puffsz);
void keccak_shake_absorb_mult(keccak_ppc64_mult *kcx, uint8_t *entry1, uint8_t *entry2, u32 entrysz);
void keccak_shake_complete_absorb_mult(keccak_ppc64_mult *kcx);
void keccak_shake_mult(keccak_ppc64_mult *kcx, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2, u32 puffsz);

void shake128_call_configure_mult(keccak_ppc64_mult *kcx);
void shake128_call_progressive_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2);

void shake256_call_configure_mult(keccak_ppc64_mult *kcx);
void shake256_call_progressive_mult(u32 puffsz, u8 *entry1, u8 *entry2, int entrysz1, int entrysz2, u8 *puff1, u8 *puff2);

#endif
