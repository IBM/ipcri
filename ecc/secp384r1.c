// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2023- IBM Corp. All rights reserved
 * Authored by Rohan McLure <rmclure@linux.ibm.com>
 */

#include <string.h>
#include <stdint.h>

typedef __uint64_t u64;
typedef __uint128_t u128;

#define NUM_LIMBS 7
typedef u64 limbs[NUM_LIMBS];
typedef u128 longlimbs[2*NUM_LIMBS-1];

/*
 * The following routines can be used in implementing secp384r1 through Solinas'
 * reduction, with the elements of the underlying field being implemented in terms of
 * 56-bit limbs (the limb ordering is little endian, but the following routines are
 * designed for arbitrary endianess).
 *
 * Link:
 * https://sthbrx.github.io/blog/2023/08/07/going-out-on-a-limb-efficient-elliptic-curve-arithmetic-in-openssl/
 */

void square(longlimbs r, const limbs a);
void mult(longlimbs r, const limbs a, const limbs b);
void reduce(limbs r, const longlimbs a);
void canonicalise(limbs r, const limbs a);

void diff_64(limbs r, const limbs a);
void diff_128(longlimbs r, const longlimbs a);
void diff_mixed(longlimbs r, const limbs a);

#define A(i) ((u128) a[i])
#define A2(i) ((u128) a2[i])
#define B(i) ((u128) b[i])

/*
 * Compute the square of a single, seven-limb, non-negative integers, returning an
 * output with redundant-limb representation, presented as thirteen,
 * double-precision limbs.
 *
 * Suppose a[i] < x,
 * Then    r[i] < 7x^2
 */
void square(longlimbs r, const limbs a) {

	unsigned i;
	limbs a2;

	for (i = 0; i < NUM_LIMBS; i++)
		a2[i] = 2 * a[i];

	r[0]  = A(0) * A(0);
	r[1]  = A(0) * A2(1);
	r[2]  = A(0) * A2(2) + A(1) * A(1);
	r[3]  = A(0) * A2(3) + A(1) * A2(2);
	r[4]  = A(0) * A2(4) + A(1) * A2(3) + A(2) * A(2);
	r[5]  = A(0) * A2(5) + A(1) * A2(4) + A(2) * A2(3);
	r[6]  = A(0) * A2(6) + A(1) * A2(5) + A(2) * A2(4) + A(3) * A(3);
	r[7]  = A(1) * A2(6) + A(2) * A2(5) + A(3) * A2(4);
	r[8]  = A(2) * A2(6) + A(3) * A2(5) + A(4) * A(4);
	r[9]  = A(3) * A2(6) + A(4) * A2(5);
	r[10] = A(4) * A2(6) + A(5) * A(5);
	r[11] = A(5) * A2(6);
	r[12] = A(6) * A(6);

}


/*
 * Multiply two, seven-limb, non-negative integers, returning an output with
 * redundant-limb representation, presented as thirteen, double-precision limbs.
 *
 * Suppose a[i] < x,
 *	   b[i] < y,
 * Then    r[i] < 7xy
 */
void mult(longlimbs r, const limbs a, const limbs b) {
	
	r[0]  = A(0) * B(0);
	r[1]  = A(0) * B(1) + A(1) * B(0);
	r[2]  = A(0) * B(2) + A(1) * B(1) + A(2) * B(0);
	r[3]  = A(0) * B(3) + A(1) * B(2) + A(2) * B(1) + A(3) * B(0);
	r[4]  = A(0) * B(4) + A(1) * B(3) + A(2) * B(2) + A(3) * B(1)
	      + A(4) * B(0);
	r[5]  = A(0) * B(5) + A(1) * B(4) + A(2) * B(3) + A(3) * B(2)
	      + A(4) * B(1) + A(5) * B(0);
	r[6]  = A(0) * B(6) + A(1) * B(5) + A(2) * B(4) + A(3) * B(3)
	      + A(4) * B(2) + A(5) * B(1) + A(6) * B(0);
	r[7]  = A(1) * B(6) + A(2) * B(5) + A(3) * B(4) + A(4) * B(3)
	      + A(5) * B(2) + A(6) * B(1);
	r[8]  = A(2) * B(6) + A(3) * B(5) + A(4) * B(4) + A(5) * B(3)
	      + A(6) * B(2);
	r[9]  = A(3) * B(6) + A(4) * B(5) + A(5) * B(4) + A(6) * B(3);
	r[10] = A(4) * B(6) + A(5) * B(5) + A(6) * B(4);
	r[11] = A(5) * B(6) + A(6) * B(5);
	r[12] = A(6) * B(6);

}


#define BITS(x, n) ((x) & (((u128) 1 << (n))-1))
#define P(e) ((u128) 1 << (e))

/*
 * Reduce a thirteen-limb, redundant representation to one with seven limbs
 * without redundancy. Limbs are interpreted to have place value equal to
 * powers of 2^56.
 *
 * a[i] < 2^128 - 2^125
 * r[i] < 2^56 for i < 6, r[6] <= 2^48
 *
 * The output value obeys the following inequality:
 * 0 <= r < 2*p384
 */
void reduce(limbs r, const longlimbs a) {
	u128 hi6, acc[9];
	unsigned i;

	memcpy(acc, a, sizeof(u128) * 9);

	acc[0] += P(124) + P(108) - P(76);
	acc[1] += P(124) - P(116) - P(68);
	acc[2] += P(124) - P(92)  - P(68);
	acc[3] += P(124) - P(68);
	acc[4] += P(124) - P(68);
	acc[5] += P(124) - P(68);
	acc[6] += P(124) - P(68);

	acc[8] += a[12] >> 32;
	acc[7] += BITS(a[12], 32) << 24;
	acc[7] += a[12] >> 8;
	acc[6] += BITS(a[12], 8)  << 48;
	acc[6] -= a[12] >> 16;
	acc[5] -= BITS(a[12], 16) << 40;
	acc[6] += a[12] >> 48;
	acc[5] += BITS(a[12], 48) << 8;

	acc[7] += a[11] >> 32;
	acc[6] += BITS(a[11], 32) << 24;
	acc[6] += a[11] >> 8;
	acc[5] += BITS(a[11], 8)  << 48;
	acc[5] -= a[11] >> 16;
	acc[4] -= BITS(a[11], 16) << 40;
	acc[5] += a[11] >> 48;
	acc[4] += BITS(a[11], 48) << 8;

	acc[6] += a[10] >> 32;
	acc[5] += BITS(a[10], 32) << 24;
	acc[5] += a[10] >> 8;
	acc[4] += BITS(a[10], 8)  << 48;
	acc[4] -= a[10] >> 16;
	acc[3] -= BITS(a[10], 16) << 40;
	acc[4] += a[10] >> 48;
	acc[3] += BITS(a[10], 48) << 8;

	acc[5] += a[9] >> 32;
	acc[4] += BITS(a[9], 32) << 24;
	acc[4] += a[9] >> 8;
	acc[3] += BITS(a[9], 8)  << 48;
	acc[3] -= a[9] >> 16;
	acc[2] -= BITS(a[9], 16) << 40;
	acc[3] += a[9] >> 48;
	acc[2] += BITS(a[9], 48) << 8;

	acc[4] += acc[8] >> 32;
	acc[3] += BITS(a[8], 32) << 24;
	acc[3] += acc[8] >> 8;
	acc[2] += BITS(a[8], 8)  << 48;
	acc[2] -= acc[8] >> 16;
	acc[1] -= BITS(a[8], 16) << 40;
	acc[2] += acc[8] >> 48;
	acc[1] += BITS(a[8], 48) << 8;

	acc[3] += acc[7] >> 32;
	acc[2] += BITS(a[7], 32) << 24;
	acc[2] += acc[7] >> 8;
	acc[1] += BITS(a[7], 8)  << 48;
	acc[1] -= acc[7] >> 16;
	acc[0] -= BITS(a[7], 16) << 40;
	acc[1] += acc[7] >> 48;
	acc[0] += BITS(a[7], 48) << 8;

	acc[5] += acc[4] >> 56;
	acc[4] = BITS(acc[4], 56);

	acc[6] += acc[5] >> 56;
	acc[5] = BITS(acc[5], 56);

	hi6 = acc[6] >> 48;
	hi6 = BITS(hi6, 48);

	acc[3] += hi6 >> 40;
	acc[2] += BITS(hi6, 40) << 16;
	acc[2] += hi6 >> 16;
	acc[1] += BITS(hi6, 16) << 40;
	acc[1] -= hi6 >> 24;
	acc[0] -= BITS(hi6, 24) << 32;
	acc[0] += hi6;

	for (i = 0; i < 6; i++) {
		acc[i+1] += acc[i] >> 56;
		r[i] = BITS(acc[i], 56);
	}

	r[6] = acc[6];

}

/*
 * Given a seven-limb, non-negative integer, |a|, obeying the inequality below.
 * Canonicalise |a| by subtracting a copy of (p384 & mask) in constant time.
 *
 * 0 <= a < 2 * p384
 *
 * The integer |a| must not contain any redundancy in its implementation, i.e.:
 * a[i] < 2^56 for i < 6, a[6] <= 2^48
 *
 * To be called on the output of reduce() where canonicalisation is necessary.
 */
void canonicalise(limbs r, const limbs a) {

	int64_t acc[NUM_LIMBS], cond[5], mask;
	unsigned i;

	memcpy(acc, a, sizeof(limbs));

	/* Case 1: a = 1 iff a >= 2^384 */
	mask = (a[6] >> 48);
	acc[0] += mask;
	acc[0] -= mask << 32;
	acc[1] += mask << 40;
	acc[2] += mask << 16;
	acc[6] &= 0x0000ffffffffffff;

	mask = acc[0] >> 63;
	acc[0] += mask & ((int64_t) 1 << 56);
	acc[1] -= mask & 1;

	for (i = 1; i < 6; i++) {
		acc[i+1] += acc[i] >> 56;
		acc[i] = BITS(acc[i], 56);
	}

	/* Case 2: a = all ones if p <= a < 2^384, 0 otherwise */
	cond[0] = ((acc[6] | 0xff000000000000)
		& acc[5] & acc[4] & acc[3]
		& (acc[2] | 0x0000000001ffff)) + 1;
	cond[1] = (acc[2] | ~0x00000000010000) + 1;
	cond[2] = ((acc[2] | 0xffffffffff0000)
		& (acc[1] | 0x0000ffffffffff)) + 1;
	cond[3] = (acc[1] & ~0xffff0000000000)
		| (acc[0] & ~((int64_t) 0x000000ffffffff));
	cond[4] = (acc[0] | 0xffffff00000000) + 1;

	for (i = 0; i < 5; i++)
		cond[i] = ((cond[i] & 0x00ffffffffffffff) - 1) >> 63;

	mask = cond[0] & (cond[1] | (cond[2] & (~cond[3] | cond[4])));
	acc[6] &= ~mask;
	acc[5] &= ~mask;
	acc[4] &= ~mask;
	acc[3] &= ~mask;
	acc[2] &= ~mask | 0x0000000001ffff;

	mask = cond[0] & cond[1];
	acc[2] &= ~mask | 0xfffffffffeffff;
	acc[1] += mask & ((int64_t) 1 << 40);

	mask = cond[0] & ~cond[1] & (cond[2] & (~cond[3] | cond[4]));
	acc[2] &= ~mask | 0xffffffffff0000;
	acc[1] &= ~mask | 0x0000ffffffffff;

	mask = cond[0] & (cond[1] | (cond[2] & (~cond[3] | cond[4])));
	acc[0] += mask & (-((int64_t) 1 << 32) + 1);

	mask = acc[0] >> 63;
	acc[0] += mask & ((int64_t) 1 << 56);
	acc[1] -= mask & 1;

	for (i = 1; i < 6; i++) {
		acc[i+1] += acc[i] >> 56;
		acc[i] = BITS(acc[i], 56);
	}

	memcpy(r, acc, sizeof(limbs));

}


void diff_64(limbs r, const limbs a) {

	unsigned i;

	r[0] += P(60) + P(44) - P(12);
	r[1] += P(60) - P(52) - P(4);
	r[2] += P(60) - P(28) - P(4);
	r[3] += P(60) - P(4);
	r[4] += P(60) - P(4);
	r[5] += P(60) - P(4);
	r[6] += P(60) - P(4);

	for (i = 0; i < NUM_LIMBS; i++)
		r[i] -= a[i];
}


void diff_128(longlimbs r, const longlimbs a) {

	unsigned i;

	r[0]  += P(127);
	r[1]  += P(127) - P(71);
	r[2]  += P(127) - P(71);
	r[3]  += P(127) - P(71);
	r[4]  += P(127) - P(71);
	r[5]  += P(127) - P(71);
	r[6]  += P(127) + P(111) - P(79) - P(71);
	r[7]  += P(127) - P(119) - P(71);
	r[8]  += P(127) - P(95) - P(71);
	r[9]  += P(127) - P(71);
	r[10] += P(127) - P(71);
	r[11] += P(127) - P(71);
	r[12] += P(127) - P(71);

	for (i = 0; i < 2*NUM_LIMBS-1; i++)
		r[i] -= a[i];

}


void diff_mixed(longlimbs r, const limbs a) {
	
	unsigned int i;
				     
	r[0] += P(64) + P(48) - P(18);
	r[1] += P(64) - P(56) - P(8);
	r[2] += P(64) - P(32) - P(8);
	r[3] += P(64) - P(8);
	r[4] += P(64) - P(8);
	r[5] += P(64) - P(8);
	r[6] += P(64) - P(8);
				     
	for (i = 0; i < NUM_LIMBS; i++)
		r[i] -= a[i];

}
