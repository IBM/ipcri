// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2024-2025 IBM Corp. All rights reserved
 * Authored by Nimet Ozkan <nozkan@linux.ibm.com>
 */

#ifndef MACRO_H
#define MACRO_H
// #include "util.h"

#define VCONST(n) ((__vector uint64_t) { n, n })
#define V0 ((__vector uint64_t) { 0, 0 })
#define V1 ((__vector uint64_t) { 1, 1 })
#define U8_ST(u64arr) ((uint8_t *) (u64arr))

#define PAD_ST(kcx, st)                                               \
	do {                                                          \
		(st)[(kcx)->sponge._threshold] ^= (kcx)->sponge._pad; \
		(st)[(kcx)->sponge._block_rate - 1] ^= KECCAK_DOM;    \
	} while (0)

// ROTL64 function in C
#define RHO0 VCONST(1)
#define RHO1 VCONST(3)
#define RHO2 VCONST(6)
#define RHO3 VCONST(10)
#define RHO4 VCONST(15)
#define RHO5 VCONST(21)
#define RHO6 VCONST(28)
#define RHO7 VCONST(36)
#define RHO8 VCONST(45)
#define RHO9 VCONST(55)
#define RHO10 VCONST(2)
#define RHO11 VCONST(14)
#define RHO12 VCONST(27)
#define RHO13 VCONST(41)
#define RHO14 VCONST(56)
#define RHO15 VCONST(8)
#define RHO16 VCONST(25)
#define RHO17 VCONST(43)
#define RHO18 VCONST(62)
#define RHO19 VCONST(18)
#define RHO20 VCONST(39)
#define RHO21 VCONST(61)
#define RHO22 VCONST(20)
#define RHO23 VCONST(44)

#define OR(c, a, b) c = (a | b)
#define XOR(c, a, b) c = (a ^ b)
#define AND(c, a, b) c = (a & b)
#define ADD(c, a, b) c = (a + b)
#define LSL(c, a, b) c = (a << b)
#define VRL(c, a, b) c = vec_rl(a, b)

#define CTHETA(out, a, b, c, d, e) \
	XOR(out, a, b);            \
	XOR(out, out, c);          \
	XOR(out, out, d);          \
	XOR(out, out, e);

#define VROTL64(c, a, b) \
	VRL(c, a, b);

#define DTHETA(c, a, b)    \
	VROTL64(c, b, V1); \
	XOR(c, c, a);

#define CHI(out, a, b, c) \
	XOR(out, a, vec_andc(c, b));

#define RHOPI(x, y, z, t) \
	x = y;            \
	VROTL64(y, z, t);

#define STATEVARS                                  \
	__vector uint64_t r0, r1, r2, r3, r4;      \
	__vector uint64_t r5, r6, r7, r8, r9;      \
	__vector uint64_t r10, r11, r12, r13, r14; \
	__vector uint64_t r15, r16, r17, r18, r19; \
	__vector uint64_t r20, r21, r22, r23, r24;

#define THETAVARS                             \
	__vector uint64_t c0, c1, c2, c3, c4; \
	__vector uint64_t d0, d1, d2, d3, d4;

#define RhoPIChiVARS \
	__vector uint64_t t0, t1, t2, t3, t4;

#define LOADST(state)            \
	r0 = VCONST(state[0]);   \
	r1 = VCONST(state[1]);   \
	r2 = VCONST(state[2]);   \
	r3 = VCONST(state[3]);   \
	r4 = VCONST(state[4]);   \
	r5 = VCONST(state[5]);   \
	r6 = VCONST(state[6]);   \
	r7 = VCONST(state[7]);   \
	r8 = VCONST(state[8]);   \
	r9 = VCONST(state[9]);   \
	r10 = VCONST(state[10]); \
	r11 = VCONST(state[11]); \
	r12 = VCONST(state[12]); \
	r13 = VCONST(state[13]); \
	r14 = VCONST(state[14]); \
	r15 = VCONST(state[15]); \
	r16 = VCONST(state[16]); \
	r17 = VCONST(state[17]); \
	r18 = VCONST(state[18]); \
	r19 = VCONST(state[19]); \
	r20 = VCONST(state[20]); \
	r21 = VCONST(state[21]); \
	r22 = VCONST(state[22]); \
	r23 = VCONST(state[23]); \
	r24 = VCONST(state[24]);

#define STOREST(state)      \
	state[0] = r0[0];   \
	state[1] = r1[0];   \
	state[2] = r2[0];   \
	state[3] = r3[0];   \
	state[4] = r4[0];   \
	state[5] = r5[0];   \
	state[6] = r6[0];   \
	state[7] = r7[0];   \
	state[8] = r8[0];   \
	state[9] = r9[0];   \
	state[10] = r10[0]; \
	state[11] = r11[0]; \
	state[12] = r12[0]; \
	state[13] = r13[0]; \
	state[14] = r14[0]; \
	state[15] = r15[0]; \
	state[16] = r16[0]; \
	state[17] = r17[0]; \
	state[18] = r18[0]; \
	state[19] = r19[0]; \
	state[20] = r20[0]; \
	state[21] = r21[0]; \
	state[22] = r22[0]; \
	state[23] = r23[0]; \
	state[24] = r24[0];

#define LOADST_MULT(state1, state2)                           \
	r0 = (__vector uint64_t) { state1[0], state2[0] };    \
	r1 = (__vector uint64_t) { state1[1], state2[1] };    \
	r2 = (__vector uint64_t) { state1[2], state2[2] };    \
	r3 = (__vector uint64_t) { state1[3], state2[3] };    \
	r4 = (__vector uint64_t) { state1[4], state2[4] };    \
	r5 = (__vector uint64_t) { state1[5], state2[5] };    \
	r6 = (__vector uint64_t) { state1[6], state2[6] };    \
	r7 = (__vector uint64_t) { state1[7], state2[7] };    \
	r8 = (__vector uint64_t) { state1[8], state2[8] };    \
	r9 = (__vector uint64_t) { state1[9], state2[9] };    \
	r10 = (__vector uint64_t) { state1[10], state2[10] }; \
	r11 = (__vector uint64_t) { state1[11], state2[11] }; \
	r12 = (__vector uint64_t) { state1[12], state2[12] }; \
	r13 = (__vector uint64_t) { state1[13], state2[13] }; \
	r14 = (__vector uint64_t) { state1[14], state2[14] }; \
	r15 = (__vector uint64_t) { state1[15], state2[15] }; \
	r16 = (__vector uint64_t) { state1[16], state2[16] }; \
	r17 = (__vector uint64_t) { state1[17], state2[17] }; \
	r18 = (__vector uint64_t) { state1[18], state2[18] }; \
	r19 = (__vector uint64_t) { state1[19], state2[19] }; \
	r20 = (__vector uint64_t) { state1[20], state2[20] }; \
	r21 = (__vector uint64_t) { state1[21], state2[21] }; \
	r22 = (__vector uint64_t) { state1[22], state2[22] }; \
	r23 = (__vector uint64_t) { state1[23], state2[23] }; \
	r24 = (__vector uint64_t) { state1[24], state2[24] };

#define STOREST_MULT(state1, state2) \
	state1[0] = r0[0];           \
	state2[0] = r0[1];           \
	state1[1] = r1[0];           \
	state2[1] = r1[1];           \
	state1[2] = r2[0];           \
	state2[2] = r2[1];           \
	state1[3] = r3[0];           \
	state2[3] = r3[1];           \
	state1[4] = r4[0];           \
	state2[4] = r4[1];           \
	state1[5] = r5[0];           \
	state2[5] = r5[1];           \
	state1[6] = r6[0];           \
	state2[6] = r6[1];           \
	state1[7] = r7[0];           \
	state2[7] = r7[1];           \
	state1[8] = r8[0];           \
	state2[8] = r8[1];           \
	state1[9] = r9[0];           \
	state2[9] = r9[1];           \
	state1[10] = r10[0];         \
	state2[10] = r10[1];         \
	state1[11] = r11[0];         \
	state2[11] = r11[1];         \
	state1[12] = r12[0];         \
	state2[12] = r12[1];         \
	state1[13] = r13[0];         \
	state2[13] = r13[1];         \
	state1[14] = r14[0];         \
	state2[14] = r14[1];         \
	state1[15] = r15[0];         \
	state2[15] = r15[1];         \
	state1[16] = r16[0];         \
	state2[16] = r16[1];         \
	state1[17] = r17[0];         \
	state2[17] = r17[1];         \
	state1[18] = r18[0];         \
	state2[18] = r18[1];         \
	state1[19] = r19[0];         \
	state2[19] = r19[1];         \
	state1[20] = r20[0];         \
	state2[20] = r20[1];         \
	state1[21] = r21[0];         \
	state2[21] = r21[1];         \
	state1[22] = r22[0];         \
	state2[22] = r22[1];         \
	state1[23] = r23[0];         \
	state2[23] = r23[1];         \
	state1[24] = r24[0];         \
	state2[24] = r24[1];

#define SET_R0_TO_R4 \
	t0 = r0;     \
	t1 = r1;     \
	t2 = r2;     \
	t3 = r3;     \
	t4 = r4;

#define SET_R5_TO_R9 \
	t0 = r5;     \
	t1 = r6;     \
	t2 = r7;     \
	t3 = r8;     \
	t4 = r9;

#define SET_R10_TO_R14 \
	t0 = r10;      \
	t1 = r11;      \
	t2 = r12;      \
	t3 = r13;      \
	t4 = r14;

#define SET_R15_TO_R19 \
	t0 = r15;      \
	t1 = r16;      \
	t2 = r17;      \
	t3 = r18;      \
	t4 = r19;

#define SET_R20_TO_R24 \
	t0 = r20;      \
	t1 = r21;      \
	t2 = r22;      \
	t3 = r23;      \
	t4 = r24;

#define KECCAKF_THETA_INIT                 \
	CTHETA(c0, r0, r5, r10, r15, r20); \
	CTHETA(c1, r1, r6, r11, r16, r21); \
	CTHETA(c2, r2, r7, r12, r17, r22); \
	CTHETA(c3, r3, r8, r13, r18, r23); \
	CTHETA(c4, r4, r9, r14, r19, r24); \
	DTHETA(d0, c4, c1);                \
	DTHETA(d1, c0, c2);                \
	DTHETA(d2, c1, c3);                \
	DTHETA(d3, c2, c4);                \
	DTHETA(d4, c3, c0);

#define KECCAKF_THETA_PROCESS \
	XOR(r0, r0, d0);      \
	XOR(r5, r5, d0);      \
	XOR(r10, r10, d0);    \
	XOR(r15, r15, d0);    \
	XOR(r20, r20, d0);    \
	XOR(r1, r1, d1);      \
	XOR(r6, r6, d1);      \
	XOR(r11, r11, d1);    \
	XOR(r16, r16, d1);    \
	XOR(r21, r21, d1);    \
	XOR(r2, r2, d2);      \
	XOR(r7, r7, d2);      \
	XOR(r12, r12, d2);    \
	XOR(r17, r17, d2);    \
	XOR(r22, r22, d2);    \
	XOR(r3, r3, d3);      \
	XOR(r8, r8, d3);      \
	XOR(r13, r13, d3);    \
	XOR(r18, r18, d3);    \
	XOR(r23, r23, d3);    \
	XOR(r4, r4, d4);      \
	XOR(r9, r9, d4);      \
	XOR(r14, r14, d4);    \
	XOR(r19, r19, d4);    \
	XOR(r24, r24, d4);

#define KECCAKF_RHOPI              \
	RHOPI(t0, r10, r1, RHO0);  \
	RHOPI(t1, r7, t0, RHO1);   \
	RHOPI(t2, r11, t1, RHO2);  \
	RHOPI(t3, r17, t2, RHO3);  \
	RHOPI(t0, r18, t3, RHO4);  \
	RHOPI(t1, r3, t0, RHO5);   \
	RHOPI(t2, r5, t1, RHO6);   \
	RHOPI(t3, r16, t2, RHO7);  \
	RHOPI(t0, r8, t3, RHO8);   \
	RHOPI(t1, r21, t0, RHO9);  \
	RHOPI(t2, r24, t1, RHO10); \
	RHOPI(t3, r4, t2, RHO11);  \
	RHOPI(t0, r15, t3, RHO12); \
	RHOPI(t1, r23, t0, RHO13); \
	RHOPI(t2, r19, t1, RHO14); \
	RHOPI(t3, r13, t2, RHO15); \
	RHOPI(t0, r12, t3, RHO16); \
	RHOPI(t1, r2, t0, RHO17);  \
	RHOPI(t2, r20, t1, RHO18); \
	RHOPI(t3, r14, t2, RHO19); \
	RHOPI(t0, r22, t3, RHO20); \
	RHOPI(t1, r9, t0, RHO21);  \
	RHOPI(t2, r6, t1, RHO22);  \
	RHOPI(t3, r1, t2, RHO23);

#define KECCAKF_CHI            \
	SET_R0_TO_R4           \
	CHI(r0, r0, t1, t2);   \
	CHI(r1, r1, t2, t3);   \
	CHI(r2, r2, t3, t4);   \
	CHI(r3, r3, t4, t0);   \
	CHI(r4, r4, t0, t1);   \
	SET_R5_TO_R9           \
	CHI(r5, r5, t1, t2);   \
	CHI(r6, r6, t2, t3);   \
	CHI(r7, r7, t3, t4);   \
	CHI(r8, r8, t4, t0);   \
	CHI(r9, r9, t0, t1);   \
	SET_R10_TO_R14         \
	CHI(r10, r10, t1, t2); \
	CHI(r11, r11, t2, t3); \
	CHI(r12, r12, t3, t4); \
	CHI(r13, r13, t4, t0); \
	CHI(r14, r14, t0, t1); \
	SET_R15_TO_R19         \
	CHI(r15, r15, t1, t2); \
	CHI(r16, r16, t2, t3); \
	CHI(r17, r17, t3, t4); \
	CHI(r18, r18, t4, t0); \
	CHI(r19, r19, t0, t1); \
	SET_R20_TO_R24         \
	CHI(r20, r20, t1, t2); \
	CHI(r21, r21, t2, t3); \
	CHI(r22, r22, t3, t4); \
	CHI(r23, r23, t4, t0); \
	CHI(r24, r24, t0, t1);

#define KECCAKF_IOTA(round) \
	XOR(r0, r0, (CROUND_TABLE[round]));

#define KECCAKF_ROUND(round)  \
	KECCAKF_THETA_INIT    \
	KECCAKF_THETA_PROCESS \
	KECCAKF_RHOPI         \
	KECCAKF_CHI           \
	KECCAKF_IOTA(round)

#define keccakf_1600_mult(state1, state2)     \
	do {                                  \
		STATEVARS;                    \
		THETAVARS;                    \
		RhoPIChiVARS;                 \
		LOADST_MULT(state1, state2);  \
		KECCAKF_ROUND(0);             \
		KECCAKF_ROUND(1);             \
		KECCAKF_ROUND(2);             \
		KECCAKF_ROUND(3);             \
		KECCAKF_ROUND(4);             \
		KECCAKF_ROUND(5);             \
		KECCAKF_ROUND(6);             \
		KECCAKF_ROUND(7);             \
		KECCAKF_ROUND(8);             \
		KECCAKF_ROUND(9);             \
		KECCAKF_ROUND(10);            \
		KECCAKF_ROUND(11);            \
		KECCAKF_ROUND(12);            \
		KECCAKF_ROUND(13);            \
		KECCAKF_ROUND(14);            \
		KECCAKF_ROUND(15);            \
		KECCAKF_ROUND(16);            \
		KECCAKF_ROUND(17);            \
		KECCAKF_ROUND(18);            \
		KECCAKF_ROUND(19);            \
		KECCAKF_ROUND(20);            \
		KECCAKF_ROUND(21);            \
		KECCAKF_ROUND(22);            \
		KECCAKF_ROUND(23);            \
		STOREST_MULT(state1, state2); \
	} while (0)

#define keccakf_1600(state)        \
	do {                       \
		STATEVARS;         \
		THETAVARS;         \
		RhoPIChiVARS;      \
		LOADST(state);     \
		KECCAKF_ROUND(0);  \
		KECCAKF_ROUND(1);  \
		KECCAKF_ROUND(2);  \
		KECCAKF_ROUND(3);  \
		KECCAKF_ROUND(4);  \
		KECCAKF_ROUND(5);  \
		KECCAKF_ROUND(6);  \
		KECCAKF_ROUND(7);  \
		KECCAKF_ROUND(8);  \
		KECCAKF_ROUND(9);  \
		KECCAKF_ROUND(10); \
		KECCAKF_ROUND(11); \
		KECCAKF_ROUND(12); \
		KECCAKF_ROUND(13); \
		KECCAKF_ROUND(14); \
		KECCAKF_ROUND(15); \
		KECCAKF_ROUND(16); \
		KECCAKF_ROUND(17); \
		KECCAKF_ROUND(18); \
		KECCAKF_ROUND(19); \
		KECCAKF_ROUND(20); \
		KECCAKF_ROUND(21); \
		KECCAKF_ROUND(22); \
		KECCAKF_ROUND(23); \
		STOREST(state);    \
	} while (0)

#endif
