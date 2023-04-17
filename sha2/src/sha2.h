// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024- IBM Corp. All rights reserved
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
#ifdef SHA512
#include "sha512.h"
#else
#include "sha256.h"
#endif

extern void SHA_FUNC(wordsz_t state[8], wordsz_t K_table[64], size_t num, uint8_t *msg);
