// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "macros.h"

#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_PURPLE "\x1b[35m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef unsigned int u32;
typedef uint8_t u8;
typedef uint64_t u64;

#define NRUN 1000
#define NLOOPS 10000000UL
#define NBLOCKS 2UL

typedef __vector unsigned long long v2_u64;
typedef __vector unsigned char v16_u8;
typedef __vector unsigned int v4_u32;

#define ERR_BAD -1
#define ERR_OK 0

#define TRY_PASS(trial)                                                                                       \
	do {                                                                                                  \
		if (!(trial)) {                                                                               \
			fprintf(stderr, "Whoops! Attempt failed: %s at %s:%d\n", #trial, __FILE__, __LINE__); \
			return;                                                                               \
		}                                                                                             \
	} while (0)

#define MEMCHECK_INT(mem, alloc)                                                                             \
	do {                                                                                                 \
		if ((mem) == NULL) {                                                                         \
			fprintf(stderr, "Whoops! Memory allocation failed at %s:%d!\n", __FILE__, __LINE__); \
			free(alloc);                                                                         \
			return ERR_BAD;                                                                      \
		}                                                                                            \
	} while (0)

#define MEMCHECK_VOID(mem, alloc)                                                                            \
	do {                                                                                                 \
		if ((mem) == NULL) {                                                                         \
			fprintf(stderr, "Whoops! Memory allocation failed at %s:%d!\n", __FILE__, __LINE__); \
			free(alloc);                                                                         \
			return;                                                                              \
		}                                                                                            \
	} while (0)

FILE *load_file(const char *fname, const char *fmode);
void up(char *str);
int parser(const char *mode);
int isshake(const char *mode);
void print_sha(char *call, const u8 *hash, char *num, u32 bits);
void print_shake(char *call, const u8 *hash, char *num, u32 bits);
void printv(v2_u64 vec);
u64 TOU64(const u8 *bytes);

#endif
