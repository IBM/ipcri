// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#include "../common/bench.h"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <function_name> <input_message>\n", argv[0]);
		return ERR_BAD;
	}

	char *mode;
	char *in;
	u32 bits;

	mode = argv[1];
	in = argv[2];
	u8 hash[1600] = { 0 };

	bits = parser(mode);

	if (bits == (u32) -1) {
		printf("Whoops, setup failed :/\n");
		return -1;
	}
	if (isshake(mode)) {
		if (bits == 128)
			shake128_call_progressive(bits, (u8 *) in, strlen(in), hash);
		else if (bits == 256)
			shake256_call_progressive(bits, (u8 *) in, strlen(in), hash);
		print_shake(mode, hash, "", bits);
	} else {
		sha3_call(bits, (u8 *) in, strlen(in), hash);
		print_sha(mode, hash, "", bits);
	}

	printf("\n SPEED: ");
	run_buff_test(mode);

	return ERR_OK;
}
