// SPDX-License-Identifier: BSD-2-Clause
/*
 *
 * Copyright 2025- IBM Corp.
 *
 * ===================================================================================
 * Written by Danny Tsen <dtsen@us.ibm.com>
 */

#include "consts.h"

#define QINV    0
#define V_Q     1
#define V_ZETA  2
#define V_Z0    2
#define V_Z1    3
#define V_Z2    4
#define V_Z3    5

.machine "any"
.text

.macro SAVE_REGS
        stdu    1, -352(1)
        mflr    0
        std     14, 56(1)
        std     15, 64(1)
        std     16, 72(1)
        std     17, 80(1)
        std     18, 88(1)
        std     19, 96(1)
        std     20, 104(1)
        std     21, 112(1)
        li      10, 128
        li      11, 144
        li      12, 160
        li      14, 176
        li      15, 192
        li      16, 208
        stxvx   32+20, 10, 1
        stxvx   32+21, 11, 1
        stxvx   32+22, 12, 1
        stxvx   32+23, 14, 1
        stxvx   32+24, 15, 1
        stxvx   32+25, 16, 1
        li      10, 224
        li      11, 240
        li      12, 256
        li      14, 272
        stxvx   32+26, 10, 1
        stxvx   32+27, 11, 1
        stxvx   32+28, 12, 1
        stxvx   32+29, 14, 1
.endm

.macro RESTORE_REGS
        li      10, 128
        li      11, 144
        li      12, 160
        li      14, 176
        li      15, 192
        li      16, 208
        lxvx    32+20, 10, 1
        lxvx    32+21, 11, 1
        lxvx    32+22, 12, 1
        lxvx    32+23, 14, 1
        lxvx    32+24, 15, 1
        lxvx    32+25, 16, 1
        li      10, 224
        li      11, 240
        li      12, 256
        li      14, 272
        lxvx    32+26, 10, 1
        lxvx    32+27, 11, 1
        lxvx    32+28, 12, 1
        lxvx    32+29, 14, 1
        ld      14, 56(1)
        ld      15, 64(1)
        ld      16, 72(1)
        ld      17, 80(1)
        ld      18, 88(1)
        ld      19, 96(1)
        ld      20, 104(1)
        ld      21, 112(1)

        mtlr    0
        addi    1, 1, 352
.endm

/*
 * Init_Coeffs_offset: initial offset setup for the coeeficient array.
 *
 * start: beginning of the offset to the coefficient array.
 * next: Next offset.
 * len: Index difference between coefficients.
 *
 * r7: len * 2, each coefficient component is 2 bytes.
 *
 * registers used for offset to coefficients, r[j] and r[j+len]
 * R9: offset to r0 = j
 * R16: offset to r1 = r0 + next
 * R18: offset to r2 = r1 + next
 * R20: offset to r3 = r2 + next
 *
 * R10: offset to r'0 = r0 + len*2
 * R17: offset to r'1 = r'0 + next
 * R19: offset to r'2 = r'1 + next
 * R21: offset to r'3 = r'2 + next
 *
 */
.macro Init_Coeffs_offset start next
        li      9, \start       /* first offset to j */
        add     10, 7, 9        /* J + len*2 */
        addi    16, 9, \next
        addi    17, 10, \next
        addi    18, 16, \next
        addi    19, 17, \next
        addi    20, 18, \next
        addi    21, 19, \next
.endm

/*
 * For Len=1, load 1-1-1-1 layout
 *
 * Load Coeffients and setup vectors
 *    rj0, rjlen1, rj2, rjlen3
 *    rj4, rjlen5, rj6, rjlen7
 *
 *  Each vmrgew and vmrgow will transpose vectors as,
 *
 *   rj vector = (rj0, rj4, rj2, rj6)
 *   rjlen vector = (rjlen1, rjlen5, rjlen3, rjlen7)
 *
 *  r' =r[j+len]: V6, V7, V8, V9
 *  r = r[j]: V26, V27, V28, V29
 *
 * In order to do the coefficients computation, zeta vector will arrange
 * in the proper order to match the multiplication.
 */
.macro Load_41Coeffs
        lxvd2x     32+10, 0, 5
        lxvd2x     32+11, 10, 5
        vmrgew 6, 10, 11
        vmrgow 26, 10, 11
        lxvd2x     32+12, 11, 5
        lxvd2x     32+13, 12, 5
        vmrgew 7, 12, 13
        vmrgow 27, 12, 13
        lxvd2x     32+10, 15, 5
        lxvd2x     32+11, 16, 5
        vmrgew 8, 10, 11
        vmrgow 28, 10, 11
        lxvd2x     32+12, 17, 5
        lxvd2x     32+13, 18, 5
        vmrgew 9, 12, 13
        vmrgow 29, 12, 13
.endm

/*
 * For Len=2, Load 2 - 2 - 2 - 2 layout
 *
 * Load Coefficients and setup vectors for 8 coefficients in the
 * following order,
 *    rj0, rj1, rjlen2, rjlen3,
 *    rj4, rj5, rjlen6, arlen7
 *  Each xxpermdi will transpose vectors as,
 *  r[j]=      rj0, rj1, rj4, rj5
 *  r[j+len]=  rjlen2, rjlen3, rjlen6, arlen7
 *
 *  r' = r[j+len]: V6, V7, V8, V9
 *  r = r[j]: V26, V27, V28, V29
 *
 * In order to do the coefficients computation, zeta vector will arrange
 * in the proper order to match the multiplication.
 */
.macro Load_42Coeffs
        lxvd2x     1, 0, 5
        lxvd2x     2, 10, 5
        xxpermdi 32+6, 1, 2, 3
        xxpermdi 32+26, 1, 2, 0
        lxvd2x     3, 11, 5
        lxvd2x     4, 12, 5
        xxpermdi 32+7, 3, 4, 3
        xxpermdi 32+27, 3, 4, 0
        lxvd2x     1, 15, 5
        lxvd2x     2, 16, 5
        xxpermdi 32+8, 1, 2, 3
        xxpermdi 32+28, 1, 2, 0
        lxvd2x     3, 17, 5
        lxvd2x     4, 18, 5
        xxpermdi 32+9, 3, 4, 3
        xxpermdi 32+29, 3, 4, 0
.endm

/*
 * For Len=8,
 * Load coefficient with 2 legs with 64  bytes apart in
 *  r[j+len] (r') vectors from offset, R10, R17, R19 and R21
 *  r[j+len]: V6, V7, V8, V9
 */
.macro Load_22Coeffs start next
        li      9, \start
        add     10, 7, 9
        addi    16, 9, \next
        addi    17, 10, \next
        li      18, \start+64
        add     19, 7, 18
        addi    20, 18, \next
        addi    21, 19, \next
        lxvd2x  32+6, 3, 10
        lxvd2x  32+7, 3, 17
        lxvd2x  32+8, 3, 19
        lxvd2x  32+9, 3, 21
.endm

/*
 * Load coefficient with 2 legs with len*2 bytes apart in
 *  r[j+len] (r') vectors from offset, R10, R17, R19 and R21
 *  r[j+len]: V6, V7, V8, V9
 */
.macro Load_4Coeffs start next
        Init_Coeffs_offset \start, \next

        lxvd2x  32+6, 3, 10
        lxvd2x  32+7, 3, 17
        lxvd2x  32+8, 3, 19
        lxvd2x  32+9, 3, 21
.endm

/*
 * Load 4 r[j] (r) coefficient vectors:
 *   Load coefficient in vectors from offset, R9, R16, R18 and R20
 *  r[j]: V26, V27, V28, V29
 */
.macro Load_4Rj
        lxvd2x  32+26, 3, 9
        lxvd2x  32+27, 3, 16
        lxvd2x  32+28, 3, 18
        lxvd2x  32+29, 3, 20
.endm

/*
 * Compute final final r[j] and r[j+len]
 *  final r[j+len]: V18, V19, V20, V21
 *  final r[j]: V22, V23, V24, V25
 */
.macro Compute_4Coeff
        vsubuwm 18, 26, 10
        vadduwm 22, 26, 10

        vsubuwm 19, 27, 11
        vadduwm 23, 27, 11

        vsubuwm 20, 28, 12
        vadduwm 24, 28, 12

        vsubuwm 21, 29, 13
        vadduwm 25, 29, 13
.endm

.macro Write_One
        stxvd2x 32+22, 3, 9
        stxvd2x 32+18, 3, 10
        stxvd2x 32+23, 3, 16
        stxvd2x 32+19, 3, 17
        stxvd2x 32+24, 3, 18
        stxvd2x 32+20, 3, 19
        stxvd2x 32+25, 3, 20
        stxvd2x 32+21, 3, 21
.endm

/*
 * Transpose the final coefficients of 2-2-2-2 layout to the orginal
 * coefficient array order.
 */
.macro PermWrite42
        Compute_4Coeff
        xxpermdi 32+10, 32+22, 32+18, 0
        xxpermdi 32+14, 32+22, 32+18, 3
        xxpermdi 32+11, 32+23, 32+19, 0
        xxpermdi 32+15, 32+23, 32+19, 3
        xxpermdi 32+12, 32+24, 32+20, 0
        xxpermdi 32+16, 32+24, 32+20, 3
        xxpermdi 32+13, 32+25, 32+21, 0
        xxpermdi 32+17, 32+25, 32+21, 3
        stxvd2x    32+10, 0, 5
        stxvd2x    32+14, 10, 5
        stxvd2x    32+11, 11, 5
        stxvd2x    32+15, 12, 5
        stxvd2x    32+12, 15, 5
        stxvd2x    32+16, 16, 5
        stxvd2x    32+13, 17, 5
        stxvd2x    32+17, 18, 5
.endm

/*
 * Transpose the final coefficients of 1-1-1-1 layout to the orginal
 * coefficient array order.
 */
.macro PermWrite41
        Compute_4Coeff
        vmrgew 10, 18, 22
        vmrgow 11, 18, 22
        vmrgew 12, 19, 23
        vmrgow 13, 19, 23
        vmrgew 14, 20, 24
        vmrgow 15, 20, 24
        vmrgew 16, 21, 25
        vmrgow 17, 21, 25
        stxvd2x 32+10, 0, 5
        stxvd2x 32+11, 10, 5
        stxvd2x 32+12, 11, 5
        stxvd2x 32+13, 12, 5
        stxvd2x 32+14, 15, 5
        stxvd2x 32+15, 16, 5
        stxvd2x 32+16, 17, 5
        stxvd2x 32+17, 18, 5
.endm

.macro Load_next_4zetas
        li      10, 16
        li      11, 32
        li      12, 48
        lxvd2x  32+V_Z0, 0, 14
        lxvd2x  32+V_Z1, 10, 14
        lxvd2x  32+V_Z2, 11, 14
        lxvd2x  32+V_Z3, 12, 14
        addi    14, 14, 64
.endm

/*
 * montgomery_reduce
 *  a = zeta * a[j+len]
 *  t = (int64_t)(int32_t)a*QINV;
 *  t = (a - (int64_t)t*Q) >> 32;
 *
 * -----------------------------------
 * MREDUCE_4X(_vz0, _vz1, _vz2, _vz3)
 */
.macro MREDUCE_4x  _vz0 _vz1 _vz2 _vz3
        /* Coefficients computation results in abosulte value of 2^64 in
           even and odd pairs */
#if defined(USE_VMULLHSW)
        /* Define USE_VMULLHSW for p10 and above, use vmulhsw and vmuluwm to compute
           higher and lower order 32 bits of the results */
        vmulhsw 18, 6, \_vz0
        vmuluwm 19, 6, \_vz0
        vmulhsw 20, 7, \_vz1
        vmuluwm 21, 7, \_vz1
        vmulhsw 22, 8, \_vz2
        vmuluwm 23, 8, \_vz2
        vmulhsw 24, 9, \_vz3
        vmuluwm 25, 9, \_vz3
#else
        vmulesw 10, 6, \_vz0
        vmulosw 11, 6, \_vz0
        vmulesw 12, 7, \_vz1
        vmulosw 13, 7, \_vz1
        vmulesw 14, 8, \_vz2
        vmulosw 15, 8, \_vz2
        vmulesw 16, 9, \_vz3
        vmulosw 17, 9, \_vz3

        /* Use vmrgrew and vmrgow to compose the upper 32 bits which
           represents >> 32 in even and odd pairs */
        vmrgew  18, 10, 11
        vmrgow  19, 10, 11
        vmrgew  20, 12, 13
        vmrgow  21, 12, 13
        vmrgew  22, 14, 15
        vmrgow  23, 14, 15
        vmrgew  24, 16, 17
        vmrgow  25, 16, 17
#endif

        /* Compute a*q^(-1) mod 2^32 and results in the upper 32 bits of
           even pair */
        vmuluwm 10, 19, QINV
        vmulesw 11, 10, V_Q
        vmulosw 12, 10, V_Q
        vmrgew  13, 11, 12
        vsubuwm 10, 18, 13

        vmuluwm 15, 21, QINV
        vmulesw 16, 15, V_Q
        vmulosw 17, 15, V_Q
        vmrgew  14, 16, 17
        vsubuwm 11, 20, 14

        vmuluwm 15, 23, QINV
        vmulesw 16, 15, V_Q
        vmulosw 17, 15, V_Q
        vmrgew  14, 16, 17
        vsubuwm 12, 22, 14

        vmuluwm 15, 25, QINV
        vmulesw 16, 15, V_Q
        vmulosw 17, 15, V_Q
        vmrgew  14, 16, 17
        vsubuwm 13, 24, 14
.endm

/*
 * For Len=8
 */
.macro NTT_MREDUCE_22x  start next _vz0 _vz1 _vz2 _vz3
        Load_22Coeffs \start, \next
        MREDUCE_4x \_vz0, \_vz1, \_vz2, \_vz3
        Load_4Rj
        Compute_4Coeff
.endm

.macro NTT_MREDUCE_4x  start next _vz0 _vz1 _vz2 _vz3
        Load_4Coeffs \start, \next
        MREDUCE_4x \_vz0, \_vz1, \_vz2, \_vz3
        Load_4Rj
        Compute_4Coeff
.endm

/*
 * mldsa_ntt_ppc(int32_t *r, const int32_t *q)
 *   Compute forward NTT based on the following 8 layers -
 *     len = 128, 64, 32, 16, 8, 4, 2, 1.
 *
 *   Each layer compute the coeffients on 2 legs, start and start + len*2 offsets.
 *
 *   leg 1                        leg 2
 *   -----                        -----
 *   start                        start+len*2
 *   start+next                   start+len*2+next
 *   start+next+next              start+len*2+next+next
 *   start+next+next+next         start+len*2+next+next+next
 *
 *   Each computation loads 8 vectors, 4 for each leg.
 *   The final coefficient (t) from each vector of leg1 and leg2 then do the
 *   add/sub operations to obtain the final results.
 *
 *   -> leg1 = leg1 + t, leg2 = leg1 - t
 *
 *   The resulting coeffients then store back to each leg's offset.
 *
 *   Each vector has the same corresponding zeta except len=2.
 *
 *   len=2 has 2-2-2-2 layout which means every 2 32-bit coeffients has the same zeta.
 *   e.g.
 *         coeff vector    a1   a2   a3  a4  a5  a6  a7  a8
 *         zeta  vector    z1   z1   z2  z2  z3  z3  z4  z4
 *
 *   For len=2, each vector will get permuted to leg1 and leg2. Zeta is
 *   pre-arranged for the leg1 and leg2.  After the computation, each vector needs
 *   to transpose back to its original 2-2-2-2 layout. 
 *
 */
.global mldsa_ntt_ppc
.align 4
mldsa_ntt_ppc:
.localentry     mldsa_ntt_ppc,.-mldsa_ntt_ppc

        SAVE_REGS

        /* load Q and QINV */
        lvx     V_Q, 0, 4
        li      10, QINV_OFFSET
        lvx     QINV, 10, 4

        /* set zetas array */
        addi      14, 4, ZETA_NTT_OFFSET

        /*
         * 1. len = 128, start = 0
         *
         *    Compute coefficients of the NTT based on 2 legs,
         *      0        -        128
         *        16        -        144
         *          32        -        160
         *                    ...
         *            112        -        240
         *     These are indexes to the 32 bits array
         *
         * r7 is len * 4
         */
        li      7, 512
        lvx     V_ZETA, 0, 14
        addi    14, 14, 16

        NTT_MREDUCE_4x 0, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one
        NTT_MREDUCE_4x 64, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one
        NTT_MREDUCE_4x 128, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one
        NTT_MREDUCE_4x 192, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one
        NTT_MREDUCE_4x 256, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one
        NTT_MREDUCE_4x 320, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one
        NTT_MREDUCE_4x 384, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one
        NTT_MREDUCE_4x 448, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_one

.align 4
        /*
         * 2. len = 64, start = 0, 128
         *
         *    Compute coefficients of the NTT based on 2 legs,
         *      0        -        64
         *        16        -        80
         *          32        -        96
         *                    ...
         *      128        -        192
         *        144        -        208
         *          160        -        224
         *            176        -        240
         *     These are indexes to the 32 bits array
         */
        li      7, 256
        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 0, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 64, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 128, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 192, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 512, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 576, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 640, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 704, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

.align 4
        /*
         * 3. len = 32, start = 0, 64, 128, 192
         *
         *    Compute coefficients of the NTT based on 2 legs,
         *      0        -        32
         *               ...
         *      64        -        96
         *               ...
         *      128        -        160
         *                ...
         *      192        -        224
         *                ...
         *
         *     These are indexes to the 32 bits array
         */
        li      7, 128
        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 0, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 64, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 256, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 320, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 512, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 576, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 768, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One
        NTT_MREDUCE_4x 832, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

.align 4
        /*
         * 4. len = 16, start = 0, 32, 64, 96, 128, 160, 192, 224
         *
         *    Compute coefficients of the NTT based on 2 legs,
         *      0        -        16
         *        32        -        48
         *          64        -        80
         *                    ...
         *            192        -        208
         *              224        -        240
         *
         *     These are indexes to the 32 bits array
         */
        li      7, 64
        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 0, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 128, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 256, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 384, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 512, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 640, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 768, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

        lvx     V_ZETA, 0, 14
        addi    14, 14, 16
        NTT_MREDUCE_4x 896, 16, V_ZETA, V_ZETA, V_ZETA, V_ZETA
        Write_One

.align 4
        /*
         * 5. len = 8, start = 0, 32, 64, 96, 128, 160, 192, 224
         *
         *    Compute coefficients of the NTT based on 2 legs,
         *      0        -        8
         *        32        -        40
         *          64        -        72
         *                    ...
         *            192        -        200
         *              224        -        232
         *
         *     These are indexes to the 32 bits array
         */

        li      7, 32
        Load_next_4zetas
        NTT_MREDUCE_22x 0, 16, V_Z0, V_Z0, V_Z1, V_Z1
        Write_One
        NTT_MREDUCE_22x 128, 16, V_Z2, V_Z2, V_Z3, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_22x 256, 16, V_Z0, V_Z0, V_Z1, V_Z1
        Write_One
        NTT_MREDUCE_22x 384, 16, V_Z2, V_Z2, V_Z3, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_22x 512, 16, V_Z0, V_Z0, V_Z1, V_Z1
        Write_One
        NTT_MREDUCE_22x 640, 16, V_Z2, V_Z2, V_Z3, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_22x 768, 16, V_Z0, V_Z0, V_Z1, V_Z1
        Write_One
        NTT_MREDUCE_22x 896, 16, V_Z2, V_Z2, V_Z3, V_Z3
        Write_One

.align 4
        /*
         * 6. len = 4, start = 0, 32, 64, 96, 128, 160, 192, 224
         *
         *    Compute coefficients of the NTT based on 2 legs,
         *      0        -        4
         *        32        -        36
         *          64        -        68
         *                    ...
         *            192        -        196
         *              224        -        228
         *
         *     These are indexes to the 32 bits array
         */

        li      7, 16

        Load_next_4zetas
        NTT_MREDUCE_4x 0, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_4x 128, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_4x 256, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_4x 384, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_4x 512, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_4x 640, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_4x 768, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

        Load_next_4zetas
        NTT_MREDUCE_4x 896, 32, V_Z0, V_Z1, V_Z2, V_Z3
        Write_One

.align 4
        /*
         * 7. len = 2, start = 0, 4, 8, 12,...244, 248, 252
         *
         *    Compute coefficients of the NTT based on 2 legs,
         *      0        -        4
         *        8        -        12
         *          16        -        20
         *                    ...
         *            240        -        244
         *              248        -        252
         *
         *     These are indexes to the 32 bits array
         */

        li      15, 4
        mtctr   15

        mr      5, 3
        li      7, 8

        li      10, 16
        li      11, 32
        li      12, 48
        li      15, 64
        li      16, 80
        li      17, 96
        li      18, 112

mldsa_ntt_ppc__Loop2:
        Load_next_4zetas
        Load_42Coeffs
        MREDUCE_4x V_Z0, V_Z1, V_Z2, V_Z3
        PermWrite42

        addi    5, 5, 128
        Load_next_4zetas
        Load_42Coeffs
        MREDUCE_4x V_Z0, V_Z1, V_Z2, V_Z3
        PermWrite42

        addi    5, 5, 128

        bdnz    mldsa_ntt_ppc__Loop2

.align 4
        /*
         * 8. len = 1, start = 0, 2, 4, 6, 8, 10, 12,...254
         *
         *    Compute coefficients of the NTT based on the following sequences,
         *      0, 1, 2, 3
         *      4, 5, 6, 7
         *      8, 9, 10, 11
         *      12, 13, 14, 15
         *            ...
         *      240, 241, 242, 243
         *      244, 245, 246, 247
         *      248, 249, 250, 251
         *      252, 253, 254, 255
         *
         *     These are indexes to the 32 bits array.  Each loads 4 vectors.
         */

        li      8, 4
        mtctr   8

        mr      5, 3
        li      7, 4

mldsa_ntt_ppc__Loop1:
        Load_next_4zetas
        Load_41Coeffs
        MREDUCE_4x V_Z0, V_Z1, V_Z2, V_Z3
        PermWrite41

        addi    5, 5, 128
        Load_next_4zetas
        Load_41Coeffs
        MREDUCE_4x V_Z0, V_Z1, V_Z2, V_Z3
        PermWrite41

        addi    5, 5, 128
        bdnz    mldsa_ntt_ppc__Loop1

        RESTORE_REGS
        blr
.size     mldsa_ntt_ppc,.-mldsa_ntt_ppc
