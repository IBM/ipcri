// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#include "src/keccak.h"

void helpmsg(char **argv)
{
	fprintf(stderr,
	    "USAGE\n"
	    "   %s <call_name> <in_len> <in_file> <out_file>\n",
	    argv[0]);
}

void digest_file(char *call, u64 len, FILE *in, FILE *out)
{
	u8 *msg = NULL;
	u8 md[200];
	u32 bits;

	msg = (u8 *) calloc(len ? len : 1, sizeof(u8));
	if (!msg) {
		printf("ERROR: Failed allocating memory for the input message");
		return;
	}

	fread(msg, sizeof(u8), len, in);
	bits = parser(call);

	if (isshake(call)) {
		if (bits == SHAKE128_DIGEST)
			shake128_call(bits, msg, len, md);
		else if (bits == SHAKE256_DIGEST)
			shake256_call(bits, msg, len, md);
	} else {
		sha3_call(bits, msg, len, md);
	}

	fwrite(md, sizeof(u8), bits / 8, out);
	free(msg);
}

int main(int argc, char **argv)
{
	if (argc < 5) {
		helpmsg(argv);
		ERR_BAD;
	}

	u64 insz;
	FILE *ifile;
	FILE *ofile;
	char *call;

	call = argv[1];
	insz = (u64) strtoll(argv[2], NULL, 10);

	ifile = load_file(argv[3], "rb");
	ofile = load_file(argv[4], "wb");

	digest_file(call, insz, ifile, ofile);

	fclose(ifile);
	fclose(ofile);

	return ERR_OK;
}
