// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#include "../common/bench.h"

int main(int argc, char *argv[])
{
	if (argc != 4) {
		printf("Usage: %s <function_name> <input_message1> <input_message2>\n", argv[0]);
		return ERR_BAD;
	}

	char *mode;
	char *in;
	char *in2;
	u32 bits;

	mode = argv[1];
	in = argv[2];
	in2 = argv[3];

	u8 hash[1600] = { 0 };
	u8 hash2[1600] = { 0 };

	bits = parser(mode);

	if (bits == (u32) -1) {
		printf("Whoops, setup failed :/\n");
		return ERR_BAD;
	}
	if (isshake(mode)) {
		if (bits == SHAKE128_DIGEST)
			shake128_call_progressive_mult(bits, (u8 *) in, (u8 *) in2, strlen(in), strlen(in2), hash, hash2);
		else if (bits == SHAKE256_DIGEST)
			shake256_call_progressive_mult(bits, (u8 *) in, (u8 *) in2, strlen(in), strlen(in2), hash, hash2);
		print_shake(mode, hash, "1", bits);
		print_shake(mode, hash2, "2", bits);
	} else {
		sha3_call_mult(bits, (u8 *) in, (u8 *) in2, strlen(in), strlen(in2), hash, hash2);
		print_sha(mode, hash, "1", bits);
		print_sha(mode, hash2, "2", bits);
	}

	printf("\n SPEED: ");
	run_buff_test_mult(mode);

	return ERR_OK;
}
