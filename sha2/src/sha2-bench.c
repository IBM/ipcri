// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>

#include "sha2.h"

#define NUM_LOOPS	10000000UL
#define NUM_BLOCKS	2UL
#define SHA_SIZE	SHA_BLOCK_SIZE

int main() {
	uint8_t buffer[SHA_SIZE * NUM_BLOCKS] = {0};
	wordsz_t state[8] = {0};
	double elapsed, tput;
	struct timespec start, end;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	for (int i = 0; i < NUM_LOOPS; i++) {
		SHA_FUNC(state, K_table, NUM_BLOCKS, buffer);
	}
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	end.tv_sec -= start.tv_sec;
	end.tv_nsec -= start.tv_nsec;

	elapsed = end.tv_sec + (1e-9 * end.tv_nsec);

	tput = ((double)(NUM_LOOPS * NUM_BLOCKS * SHA_SIZE)) / elapsed;
	tput /= 1000000;


	printf("Ran %lu loops w/ %lu blocks -> %lu compresses in %fs\n", NUM_LOOPS, NUM_BLOCKS, NUM_LOOPS * NUM_BLOCKS, elapsed);
	printf("Throughput ≈ %f MB/s\n", tput);
}
