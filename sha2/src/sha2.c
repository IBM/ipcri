// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <assert.h>

#include "sha2.h"

static int load_data(uint8_t **buf, size_t *datasz, char *filename)
{
	FILE *fp;

	int rc = 0;

	// TODO: move to a test case, here just for sanity
	// NOTE: input data is assume to be byte-aligned. It's not worth
	//   trying to handle arbitrary bit-sized inputs
#ifdef SHA512
	assert(get_bufsz(0) == 128);
	assert(get_bufsz(1) == 128);
	assert(get_bufsz(111) == 128);
	assert(get_bufsz(112) == 256);
	assert(get_bufsz(128) == 256);
#else
	assert(get_bufsz(0) == 64);
	assert(get_bufsz(1) == 64);
	assert(get_bufsz(55) == 64);
	assert(get_bufsz(56) == 128);
	assert(get_bufsz(64) == 128);
#endif

	if (!(fp = fopen(filename, "r"))) {
		printf("failed to open file '%s'\n", filename);
		rc = 1;
		goto out;
	}

	if ((rc = fseek(fp, 0, SEEK_END))) {
		printf("failed to seek file: %d\n", rc);
		rc = 2;
		goto out;
	}

	*datasz = ftell(fp);
	rewind(fp);

	*buf = calloc(1, get_bufsz(*datasz));
	if (!*buf) {
		printf("failed to allocate memory, rc = %d\n", rc);
		rc = 3;
		goto out;
	}


	if (*datasz != fread(*buf, 1, *datasz, fp)) {
		printf("probably didn't read in the whole file, panicking anyway\n");
		rc = 4;
		goto out;
	}

	fclose(fp);

out:
	return rc;
}



int main(int argc, char **argv)
{
	int rc = 0;
	uint8_t *buf;
	size_t datasz;

	wordsz_t state[8] = {0};

	if (argc != 2) {
		printf("%s <file>\n", argv[0]);
		rc = 1;
		goto out;
	}

	// Load
	load_data(&buf, &datasz, argv[1]);

	// Pad
	buf[datasz] |= 0x80;
	// NOTE: large buffer sizes (sha512, >MAX_UINT) will not work. Not worth implementing.
	*((uint64_t*) (buf + get_bufsz(datasz) - 8)) = htobe64((uint64_t) datasz * 8);

	assert(get_bufsz(datasz) % SHA_BLOCK_SIZE == 0);

	// Compress
	memcpy(state, initial_state, sizeof(initial_state));

	SHA_FUNC(state, K_table, get_bufsz(datasz) / SHA_BLOCK_SIZE, buf);

	// Output
	for (int i = 0; i < 8; i++) {
#ifdef SHA512
		printf("%016lx", state[i]);
#else
		printf("%08x", state[i]);
#endif
	}
	printf("\n");

out:
	return rc;
}
