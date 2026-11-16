// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright IBM Corp. 2026
 * Authored by Eric Richter <erichte@linux.ibm.com>
 */
#ifndef _future_h_
#define _future_h_

#ifndef COMPILER_FEATURES
#warning COMPILER_FEATURES not defined, assuming no compiler support
#define COMPILER_FEATURES	0
#endif

#define NOT_SUPPORTED(R) (!(COMPILER_FEATURES&R))


#define RFC_02653	1 << 0 /* Dense Math Facility */
#define RFC_02654	1 << 1 /* SHA2 & SHA3 Compute Instructions */
#define RFC_02657	1 << 2 /* AES Acceleration */
#define RFC_02680	1 << 3 /* SIMD for PQC Acceleration */

#define PPC_32_SH(bpos, len) (31-(bpos+len-1))

#define X(PO, XO, AT, AB, N)    \
	.long                   \
	( PO << PPC_32_SH(0,6)  \
	| AT << PPC_32_SH(6,3)  \
	| N << PPC_32_SH(11,5)  \
	| AB << PPC_32_SH(16,3) \
	| XO << PPC_32_SH(21,10))

#define XX2_SHAPAD(PO, XO, AT, XB, ID, E, BL)                  \
	.long                                                  \
	( PO << PPC_32_SH(0, 6)  | AT << PPC_32_SH(6, 3)       \
	| ID << PPC_32_SH(11, 2) | E << PPC_32_SH(13, 1)       \
	| BL << PPC_32_SH(14, 2) | (XB%32) << PPC_32_SH(16, 5) \
	| XO << PPC_32_SH(21, 9) | (XB/32) << PPC_32_SH(30, 1))

#define XX2(PO, XO, XTp, XBp, M)                                              \
	.long                                                                 \
	( PO << PPC_32_SH(0, 6)            | ((XTp%32)/2) << PPC_32_SH(6, 4)  \
	| (XTp/32) << PPC_32_SH(10, 1)     | (M&0b10) << PPC_32_SH(15, 2)     \
	| ((XBp%32)/2) << PPC_32_SH(16, 4) | (M&0b01) << PPC_32_SH(20, 1)     \
	| XO << PPC_32_SH(21, 9)           | (XBp/32) << PPC_32_SH(30, 1))

#define XX3(PO, XO, XTp, XAp, XBp, M)                                     \
	.long                                                             \
	( PO << PPC_32_SH(0, 6)        | ((XTp%32)/2) << PPC_32_SH(6, 4)  \
	| (XTp/32) << PPC_32_SH(10, 1) | ((XAp%32)/2) << PPC_32_SH(11, 4) \
	| (M&0b10) << PPC_32_SH(15, 2) | ((XBp%32)/2) << PPC_32_SH(16, 4) \
	| (M&0b01) << PPC_32_SH(20, 1) | XO << PPC_32_SH(21, 8)           \
	| (XAp/32) << PPC_32_SH(29, 1) | (XBp/32) << PPC_32_SH(30, 1))

#define XX3_P(PO, XO, AT, XAp, XBp, P)                                    \
	.long                                                             \
	( PO << PPC_32_SH(0, 6)            | AT << PPC_32_SH(6, 3)        \
	| ((XAp%32)/2) << PPC_32_SH(11, 4) | P << PPC_32_SH(15, 1)        \
	| ((XBp%32)/2) << PPC_32_SH(16, 4) | XO << PPC_32_SH(21, 8)       \
	| (XAp/32) << PPC_32_SH(29, 1)     | (XBp/32) << PPC_32_SH(30, 1))\

#define XX3_P2(PO, XO, AT, XBp, P)                                     \
	.long                                                          \
	( PO << PPC_32_SH(0, 6)     | AT << PPC_32_SH(6, 3)            \
	| (P/2) << PPC_32_SH(15, 1) | ((XBp%32)/2) << PPC_32_SH(16, 4) \
	| (P%2) << PPC_32_SH(20, 1) | XO << PPC_32_SH(21, 9)           \
	| (XBp/32) << PPC_32_SH(30, 1))

#define XX3_GF(PO, XO, XT, XA, XB, P)                               \
	.long                                                       \
	( PO << PPC_32_SH(0, 6)       | (XT%32) << PPC_32_SH(6, 5)  \
	| (XA%32) << PPC_32_SH(11, 5) | (XB%32) << PPC_32_SH(16, 5) \
	| 3 << PPC_32_SH(21, 2)       | P << PPC_32_SH(23, 1)       \
	| XO << PPC_32_SH(24, 5)      | (XA/32) << PPC_32_SH(29, 1) \
	| (XB/32) << PPC_32_SH(30, 1) | (XT/32) << PPC_32_SH(31, 1))

#define X_SHA2(PO, XO, AT, AB, N)                          \
	.long                                              \
	( PO << PPC_32_SH(0, 6)  | AT << PPC_32_SH(6, 3)   \
	| N << PPC_32_SH(10, 1)  | 14 << PPC_32_SH(11, 5)  \
	| AB << PPC_32_SH(16, 3) | XO << PPC_32_SH(21, 10))

#define X_SHA3(PO, XO, ATp, SR)                            \
	.long                                              \
	(PO << PPC_32_SH(0, 6)   | ATp << PPC_32_SH(6, 2)  \
	| 15 << PPC_32_SH(11, 5) | SR << PPC_32_SH(16, 5)  \
	| XO << PPC_32_SH(21, 10))

#define XX3_XV(PO, XO, XT, XA, XB)                                  \
	.long                                                       \
	( PO      << PPC_32_SH(0, 6)  | (XT%32) << PPC_32_SH(6, 5)  \
	| (XA%32) << PPC_32_SH(11, 5) | (XB%32) << PPC_32_SH(16, 5) \
	| XO      << PPC_32_SH(21, 8) | (XA/32) << PPC_32_SH(29, 1) \
	| (XB/32) << PPC_32_SH(30, 1) | (XT/32) << PPC_32_SH(31, 1))

#define DMSETDMRZ(AT)                   X(31, 177, AT, 0, 2)
#define DMMR(AT, AB)                    X(31, 177, AT, AB, 6)
#define DMXXINSTDMR512(AT, XAp, XBp, P) XX3_P(60, 234, AT, XAp, XBp, P)
#define DMXXEXTFDMR512(XAp, XBp, AS, P) XX3_P(60, 226, AS, XAp, XBp, P)
#define DMXXINSTDMR256(AT, XBp, P)      XX3_P2(60, 485, AT, XBp, P)
#define DMXXEXTFDMR256(XBp, AS, P)      XX3_P2(60, 484, AS, XBp, P)

#define DMSHA2HASH(AT, AB, N)           X_SHA2(31, 177, AT, AB, N)
#define DMSHA256HASH(AT, AB)            X_SHA2(31, 177, AT, AB, 0)
#define DMSHA512HASH(AT, AB)            X_SHA2(31, 177, AT, AB, 1)

#define DMSHA3HASH(ATp, SR)             X_SHA3(31, 177, ATp, SR)
#define DMSHA3DW(ATp)                   X_SHA3(31, 177, ATp, 0)
#define DMCRYSHASH(ATp)                 X_SHA3(31, 177, ATp, 12)

#define DMXXSHAPAD(AT, XB, ID, E, BL)   XX2_SHAPAD(60, 421, AT, XB, ID, E, BL)
#define DMXXSHA3512PAD(AT, XB, E)       DMXXSHAPAD(AT, XB, 0b00, E, 0b00)
#define DMXXSHA3384PAD(AT, XB, E)       DMXXSHAPAD(AT, XB, 0b00, E, 0b01)
#define DMXXSHA3256PAD(AT, XB, E)       DMXXSHAPAD(AT, XB, 0b00, E, 0b10)
#define DMXXSHA3224PAD(AT, XB, E)       DMXXSHAPAD(AT, XB, 0b00, E, 0b11)
#define DMXXSHAKE256PAD(AT, XB, E)      DMXXSHAPAD(AT, XB, 0b01, E, 0b10)
#define DMXXSHAKE128PAD(AT, XB, E)      DMXXSHAPAD(AT, XB, 0b01, E, 0b11)
#define DMXXSHA384512PAD(AT, XB)        DMXXSHAPAD(AT, XB, 0b10, 0, 0b00)
#define DMXXSHA2224256PAD(AT, XB)       DMXXSHAPAD(AT, XB, 0b11, 0, 0b00)

#define XXAESENCP(XTp, XAp, XBp, M)     XX3(60, 194, XTp, XAp, XBp, M)
#define XXAES128ENCP(XTp, XAp, XBp)     XXAESENCP(XTp, XAp, XBp, 0)
#define XXAES192ENCP(XTp, XAp, XBp)     XXAESENCP(XTp, XAp, XBp, 1)
#define XXAES256ENCP(XTp, XAp, XBp)     XXAESENCP(XTp, XAp, XBp, 2)

#define XXAESDECP(XTp, XAp, XBp, M)     XX3(60, 202, XTp, XAp, XBp, M)
#define XXAES128DECP(XTp, XAp, XBp)     XXAESDECP(XTp, XAp, XBp, 0)
#define XXAES192DECP(XTp, XAp, XBp)     XXAESDECP(XTp, XAp, XBp, 1)
#define XXAES256DECP(XTp, XAp, XBp)     XXAESDECP(XTp, XAp, XBp, 2)

#define XXAESGENLKP(XTp, XBp, M)        XX2(60, 420, XTp, XBp, M)
#define XXAES128GENLKP(XTp, XBp)        XXAESGENLKP(XTp, XBp, 0b00)
#define XXAES192GENLKP(XTp, XBp)        XXAESGENLKP(XTp, XBp, 0b01)
#define XXAES256GENLKP(XTp, XBp)	XXAESGENLKP(XTp, XBp, 0b10)

#define XXGFMUL128(XT, XA, XB, P)       XX3_GF(60, 26, XT, XA, XB, P)
#define XXGFMUL128GCM(XT, XA, XB)       XXGFMUL128(XT, XA, XB, 0)
#define XXGFMUL128XTS(XT, XA, XB)       XXGFMUL128(XT, XA, XB, 1)

#define XVADDUWM(XT, XA, XB)		XX3_XV(60, 131, XT, XA, XB)
#define XVSUBUWM(XT, XA, XB)		XX3_XV(60, 147, XT, XA, XB)


#if NOT_SUPPORTED(RFC_02653)
.macro dmsetdmrz AT
DMSETDMRZ(\AT)
.endm

.macro dmmr AT, AB
DMMR(\AT, \AB)
.endm

.macro dmxxinstdmr512 AT, XAp, XBp, P
DMXXINSTDMR512(\AT, \XAp, \XBp, \P)
.endm

.macro dmxxextfdmr512 XAp, XBp, AS, P
DMXXEXTFDMR512(\XAp, \XBp, \AS, \P)
.endm

.macro dmxxinstdmr256 AT, XAp, P
DMXXINSTDMR256(\AT, \XAp, \P)
.endm

.macro dmxxextfdmr256 XBp, AS, P
DMXXEXTFDMR256(\XBp, \AS, \P)
.endm
#endif

#if NOT_SUPPORTED(RFC_02654)
.macro dmsha2hash AT, AB, N
DMSHA2HASH(\AT, \AB, \N)
.endm

.macro dmsha256hash AT, AB
DMSHA256HASH(\AT, \AB)
.endm

.macro dmsha512hash AT, AB
DMSHA512HASH(\AT, \AB)
.endm

.macro dmsha3hash ATp, SR
DMSHA3HASH(\ATp, \SR)
.endm

.macro dmsha3dw ATp
DMSHA3DW(\ATp)
.endm

.macro dmcryshash ATp
DMCRYSHASH(\ATp)
.endm

.macro dmxxshapad AT, XB, ID, E, BL
DMXXSHAPAD(\AT, \XB, \ID, \E, \BL)
.endm

.macro dmxxsha3512pad AT, XB, E
DMXXSHA3512PAD(\AT, \XB, \E)
.endm

.macro dmxxsha3384pad AT, XB, E
DMXXSHA3384PAD(\AT, \XB, \E)
.endm

.macro dmxxsha3256pad AT, XB, E
DMXXSHA3256PAD(\AT, \XB, \E)
.endm

.macro dmxxsha3224pad AT, XB, E
DMXXSHA3224PAD(\AT, \XB, \E)
.endm

.macro dmxxshake256pad AT, XB, E
DMXXSHAKE256PAD(\AT, \XB, \E)
.endm

.macro dmxxshake128pad AT, XB, E
DMXXSHAKE128PAD(\AT, \XB, \E)
.endm

.macro dmxxsha384512pad AT, XB
DMXXSHA384512PAD(\AT, \XB)
.endm

.macro dmxxsha2224256pad AT, XB
DMXXSHA2224256PAD(\AT, \XB)
.endm
#endif

#if NOT_SUPPORTED(RFC_02657)
.macro xxaesencp XTp, XAp, XBp, M
XXAESENCP(\XTp, \XAp, \XBp, \M)
.endm

.macro xxaesdecp XTp, XAp, XBp, M
XXAESDECP(\XTp, \XAp, \XBp, \M)
.endm

.macro xxaesgenlkp XTp, XBp, M
XXAESGENLKP(\XTp, \XBp, \M)
.endm

.macro xxaes128genlkp XTp, XBp
XXAES128GENLKP(\XTp, \XBp)
.endm

.macro xxaes192genlkp XTp, XBp
XXAES192GENLKP(\XTp, \XBp)
.endm

.macro xxaes256genlkp XTp, XBp
XXAES256GENLKP(\XTp, \XBp)
.endm

.macro xxgfmul128 XT, XA, XB, P
XXGFMUL128(\XT, \XA, \XB, \P)
.endm

.macro xxgfmul128gcm XT, XA, XB
XXGFMUL128GCM(\XT, \XA, \XB)
.endm

.macro xxgfmul128xts XT, XA, XB
XXGFMUL128XTS(\XT, \XA, \XB)
.endm
#endif

#if NOT_SUPPORTED(RFC_02680)
.macro xvadduwm XT, XA, XB
XVADDUWM(\XT, \XA, \XB)
.endm

.macro xvsubuwm XT, XA, XB
XVSUBUWM(\XT, \XA, \XB)
.endm
#endif

#endif
