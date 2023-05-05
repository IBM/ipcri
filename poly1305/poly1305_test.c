// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2023- IBM Corp. All rights reserved
 *
 * This program tests various base (26, 44 and 64)-bits limbs multiplication
 * with RFC7539 test vectors.
 *
 * ===================================================================================
 * Written by Danny Tsen <dtsen@us.ibm.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "test_vectors.h"

#define POLY1305_BLOCK_SIZE	16

typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned char u8;

typedef struct
{
	u32 k[4];
	u32 r[4];
	u32 h[5];
	u32 pad;
} POLY1305_STATE;

unsigned int poly1305_p10le_4blocks(u32 *k, u8 *in, u32 len);
void poly1305_64s(u32 *k, u8 *in, u32 len, int highbit);
void poly1305_emit_64(u32 *h, u32 *r, u8 *mac);
void poly1305_44(u32 *k, u8 *in, int len, int highbit);
void Poly1305_emit_44(u32 *h, u32 *r, u8 *mac);

void Do_poly1305_44(POLY1305_STATE *pst, u8 *plaintext, u32 psize, u8 *mac)
{
	if (psize >= POLY1305_BLOCK_SIZE) {
		poly1305_44(pst->k, plaintext, psize, 1);
		plaintext += psize - (psize % 16);
		psize %= 16;
	}

	if (psize > 0) {
		unsigned char buf[16];
		memset(buf, 0, 16);
		memcpy(buf, plaintext, psize);
		buf[psize] = 1;
		poly1305_44(pst->k, buf, 16, 0);
	}

	Poly1305_emit_44(pst->h, pst->r, mac);
}

void Do_poly1305_26(POLY1305_STATE *pst, u8 *plaintext, u32 psize, u8 *mac)
{
	if (psize >= POLY1305_BLOCK_SIZE*4) {
		poly1305_p10le_4blocks(pst->k, plaintext, psize);
		plaintext += psize - (psize % (POLY1305_BLOCK_SIZE * 4));
		psize %= POLY1305_BLOCK_SIZE * 4;
	}
	while (psize >= POLY1305_BLOCK_SIZE) {
		poly1305_64s(pst->k, plaintext, POLY1305_BLOCK_SIZE, 1);
		psize -= POLY1305_BLOCK_SIZE;
		plaintext += POLY1305_BLOCK_SIZE;
	}

	if (psize > 0) {
		unsigned char buf[16];
		memset(buf, 0, 16);
		memcpy(buf, plaintext, psize);
		buf[psize] = 1;
		poly1305_64s(pst->k, buf, 16, 0);
	}

	poly1305_emit_64(pst->h, pst->r, mac);
}

void Do_poly1305_64(POLY1305_STATE *pst, u8 *plaintext, u32 psize, u8 *mac)
{
	if (psize >= POLY1305_BLOCK_SIZE) {
		poly1305_64s(pst->k, plaintext, psize, 1);
		plaintext += psize - (psize % 16);
		psize %= 16;
	}

	if (psize > 0) {
		unsigned char buf[16];
		memset(buf, 0, 16);
		memcpy(buf, plaintext, psize);
		buf[psize] = 1;
		poly1305_64s(pst->k, buf, 16, 0);
	}

	poly1305_emit_64(pst->h, pst->r, mac);
}

void RFC7539_testvec(int bits)
{
        struct hash_testvec *htv = &poly1305_tv_template[1];
	POLY1305_STATE pst ={0};
        u8 *key;
        u8 *plaintext = htv->plaintext;
        unsigned int psize = htv->psize;
        u8 *digest = htv->digest;
        unsigned char mac[24];
        int i;

	printf("-------  Start RFC7539 Test -----------------------------\n");
        printf("==== RFC7539 test vector, with %d Bits multiplication ====\n", bits);

        for (i = 0; i < 11; i++) {
                printf("Test vector: %d\n", i+1);
                htv = &poly1305_tv_template[i];
                plaintext = htv->plaintext;
                psize = htv->psize;

                key = plaintext;

		plaintext += 32;
		psize -= 32;
		memset(pst.k, 0, sizeof(POLY1305_STATE));
		memcpy(pst.k, (unsigned char *)key, 32);
		switch (bits) {
		case 26:
			Do_poly1305_26(&pst, plaintext, psize, mac);
			break;
		case 64:
			Do_poly1305_64(&pst, plaintext, psize, mac);
			break;
		case 44:
			Do_poly1305_44(&pst, plaintext, psize, mac);
			break;
		default:
			printf("Invalid option\n");
			return;
		}

		if (memcmp(mac, htv->digest, 16) == 0) {
			printf(" Vector %d Verified\n", i+1);
		}
		printf("----------------------------------\n");
        }
}

int main( int argc, char *varg[])
{
	int bits = 26;
	if (argc > 1) {
		bits = atoi(varg[1]);
	}

	RFC7539_testvec(bits);

	exit (0);
}

