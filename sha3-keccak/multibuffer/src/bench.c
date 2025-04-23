// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#include "../../common/bench.h"

void run_permspeed_test_mult()
{
	struct timeval tic, toc;
	double timeis = 0;
	unsigned int i;

	u64 state1[STATE_SIZE];
	u64 state2[STATE_SIZE];

	srand((unsigned) time(NULL));

	for (i = 0; i < STATE_SIZE; i++) {
		state1[i] = ((uint64_t) rand() << 32) | rand();
		state2[i] = ((uint64_t) rand() << 32) | rand();
	}

	printf("\n\033[1;33mRunning speed test...\033[0m\n");

	gettimeofday(&tic, NULL);
	for (i = 0; i < NLOOPS; i++) {
		keccakf_1600_mult(state1, state2);
	}
	gettimeofday(&toc, NULL);

	timeis = toc.tv_sec - tic.tv_sec + 1E-6 * (toc.tv_usec - tic.tv_usec);
	timeis /= 1000000;

	printf("Ran %u times in %f seconds with throughput %f \n\n", i,
	    (double) (toc.tv_usec - tic.tv_usec) / 1000000 + (double) (toc.tv_sec - tic.tv_sec),
	    timeis);
}

void run_buff_test_mult(char *mode)
{
	u32 buffer_sizes[] = { 16, 64, 256, 1024, 8192, 10240, 16384 };
	u32 i, j;
	u32 bits;
	u32 tot_buff;
	u32 in_len;
	u8 out[KECCAK_BITS_TOTAL];
	u8 out2[KECCAK_BITS_TOTAL];
	u8 *dummy_msg;

	struct timespec start_, end_;
	double thput, elapsed;

	bits = parser(mode);
	tot_buff = sizeof(buffer_sizes) / sizeof(buffer_sizes[0]);

	printf("\n\033[1;33mRunning buffer test...\033[0m\n");
	printf(ANSI_COLOR_CYAN "%-14s" ANSI_COLOR_RESET "%s" ANSI_COLOR_PURPLE
			       "%-15s" ANSI_COLOR_RESET "\n",
	    "buffer size", "", "speed");

	for (i = 0; i < tot_buff; i++) {
		in_len = buffer_sizes[i];
		dummy_msg = malloc(in_len * sizeof(u8));
		memset(dummy_msg, 0, in_len);

		if (isshake(mode)) {
			if (bits == SHAKE128_DIGEST) {
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_);
				for (j = 0; j < 1000; j++) {
					shake128_call_progressive_mult(bits, dummy_msg, dummy_msg, in_len, in_len, out, out2);
				}
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_);
			} else if (bits == SHAKE256_DIGEST) {
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_);
				for (j = 0; j < 1000; j++) {
					shake256_call_progressive_mult(bits, dummy_msg, dummy_msg, in_len, in_len, out, out2);
				}
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_);
			}
		} else {
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_);
			for (j = 0; j < 1000; j++) {
				sha3_call_mult(bits, dummy_msg, dummy_msg, in_len, in_len, out, out2);
			}
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_);
		}
		end_.tv_sec -= start_.tv_sec;
		end_.tv_nsec -= start_.tv_nsec;

		elapsed = end_.tv_sec + (1e-9 * end_.tv_nsec);
		thput = (in_len * NRUN) / (elapsed * 1000.0);

		printf(ANSI_COLOR_CYAN
		    "%-14u" ANSI_COLOR_RESET ANSI_COLOR_PURPLE
		    "%-15.6f" ANSI_COLOR_RESET "\n",
		    in_len, thput);
	}
	printf("\n");
	run_permspeed_test_mult();
}
