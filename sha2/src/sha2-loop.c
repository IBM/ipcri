// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
#include <stdint.h>
#include <stddef.h>

#include "sha2.h"

#define NUM_LOOPS	1000000
#define NUM_BLOCKS	2
#define SHA_SIZE	SHA_BLOCK_SIZE

// state k blocks data
int main() {
	uint8_t buffer[SHA_SIZE * NUM_BLOCKS] = {0};
	wordsz_t state[8] = {0};

	for (int i = 0; i < NUM_LOOPS; i++) {
		SHA_FUNC(state, K_table, NUM_BLOCKS, buffer);
	}
}
