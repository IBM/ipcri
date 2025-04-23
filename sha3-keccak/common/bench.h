// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#ifndef BENCH_H
#define BENCH_H

#include "../multibuffer/src/keccak_mult.h"
#include "../singlebuffer/src/keccak.h"

void run_buff_test(char *mode);
void run_permspeed_test(void);
void run_buff_test_mult(char *mode);
void run_permspeed_test_mult(void);

#endif
