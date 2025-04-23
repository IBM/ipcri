// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#include "util.h"

FILE *load_file(const char *fname, const char *fmode)
{
	FILE *file;

	file = fopen(fname, fmode);

	if (!file) {
		perror("Error opening file");
		printf(" %s\n", fname);
		exit(EXIT_FAILURE);
	}
	return file;
}

void up(char *str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		str[i] = toupper(str[i]);
	}
}

int isshake(const char *mode)
{
	int i;
	char word[100];
	strcpy(word, mode);

	for (i = 0; word[i]; i++) {
		word[i] = tolower(word[i]);
	}
	return strstr(word, "shake") != NULL;
}

void print_sha(char *call, const u8 *hash, char *num, u32 bits)
{
	u32 i;
	printf("Hash%s (%s): ", num, call);

	for (i = 0; i < bits / 8; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n\n");
}

void print_shake(char *call, const u8 *hash, char *num, u32 bits)
{
	u32 i;
	printf("Hash%s (%s): ", num, call);

	for (i = 0; i < bits / 8 * 2; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n\n");
}

int parser(const char *mode)
{
	const char *delim;
	u32 outsz;

	delim = strchr(mode, '-');
	if (delim == NULL) {
		printf("Invalid input format!\n");
		return EXIT_FAILURE;
	}

	outsz = atoi(delim + 1);
	if (outsz == 0) {
		printf("Invalid output size!\n");
		return EXIT_FAILURE;
	}
	return outsz;
}

void printv(v2_u64 vec)
{
	printf("Vector values: %llu, %llu\n",
	    vec[0],
	    vec[1]);
}
