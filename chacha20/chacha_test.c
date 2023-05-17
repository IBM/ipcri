// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2023- IBM Corp. All rights reserved
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "test_vectors.h"

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void chacha_p10le_8x(u32 *state, u8 *dst, u8 *src, size_t len, int nrounds);

enum permx {
	PERMX10 = 0x22330011U,
	PERMX11 = 0x66774455U,
	PERMX12 = 0xaabb8899U,
	PERMX13 = 0xeeffccddU,
	PERMX20 = 0x11223300U,
	PERMX21 = 0x55667744U,
	PERMX22 = 0x99aabb88U,
	PERMX23 = 0xddeeffccU
};

enum chacha_constants { /* expand 32-byte k */
	CHACHA_CONSTANT_EXPA = 0x61707865U,
	CHACHA_CONSTANT_ND_3 = 0x3320646eU,
	CHACHA_CONSTANT_2_BY = 0x79622d32U,
	CHACHA_CONSTANT_TE_K = 0x6b206574U
};

static u32 UTO32LE(unsigned char *p)
{
        return ((u32) p[3] << 24) | ((u32) p[2] << 16) | ((u32) p[1] << 8) | (u32) p[0];
}

static u32 UTO32I(unsigned char *p)
{
        return ((u32) p[0] << 0) | ((u32) p[1] << 8) | ((u32) p[2] << 16) | ((u32) p[3] << 24);
}

static inline void chacha_init_consts(u32 *state)
{
	state[0]  = CHACHA_CONSTANT_EXPA;
	state[1]  = CHACHA_CONSTANT_ND_3;
	state[2]  = CHACHA_CONSTANT_2_BY;
	state[3]  = CHACHA_CONSTANT_TE_K;
}

/*
 * The state setup can be done in asembly code's data section.
 */
static inline void chacha_init_generic(u32 *state, u8 *key, u8 *iv)
{
	chacha_init_consts(state);
	state[4]  = UTO32LE(key + 0);
	state[5]  = UTO32LE(key + 4);
	state[6]  = UTO32LE(key + 8);
	state[7]  = UTO32LE(key + 12);
	state[8]  = UTO32LE(key + 16);
	state[9]  = UTO32LE(key + 20);
	state[10]  = UTO32LE(key + 24);
	state[11]  = UTO32LE(key + 28);
	state[12] = UTO32I(iv +  0);
	state[13] = UTO32LE(iv +  4);
	state[14] = UTO32LE(iv +  8);
	state[15] = UTO32LE(iv + 12);
}

int main( int argc, char *varg[])
{
	struct cipher_testvec *ctv;
	u8 *plaintext;
	u8 *output;
	int len;
	u32 state[16];
	int nrounds = 20;
	int n = ARRAY_SIZE(chacha20_tv_template);
	int i;

	printf("==== Chacha20 tests with RFC7539 test vectors. ====\n");
	for (i = 0; i < n; i++) {
		ctv = &chacha20_tv_template[i];
		plaintext = ctv->ptext;
		len = ctv->len;
		chacha_init_generic(state, ctv->key, ctv->iv);

		output = malloc(len);
		memset(output, 0, len);

		chacha_p10le_8x(state, output, (unsigned char *) plaintext, len, nrounds);

		if (memcmp(output, ctv->ctext, len) == 0)
			printf("Vector %d Verified\n", i);
		else
			printf("Test vector %d Failed\n", i);
		free(output);
    }

    exit (0);
}

