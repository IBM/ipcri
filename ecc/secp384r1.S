# SPDX-License-Identifier: BSD-2-Clause
# Copyright 2023- IBM Corp. All rights reserved
# Authored by Rohan McLure <rmclure@linux.ibm.com>

.machine "any"
.text

.globl mult
.align 5
mult:
    xxspltib  v0,0

    # load a
    lxsd      v12,0(r4)
    lxsd      v13,8(r4)
    lxsd      v14,16(r4)
    lxsd      v15,24(r4)
    lxsd      v16,32(r4)
    lxsd      v17,40(r4)
    lxsd      v18,48(r4)

    # load b
    lxsd      v3,0(r5)
    lxsd      v4,8(r5)
    lxsd      v5,16(r5)
    lxsd      v6,24(r5)
    lxsd      v7,32(r5)
    lxsd      v8,40(r5)
    lxsd      v9,48(r5)

    # r[0]
    vmsumudm  v19,v12,v3,v0
    stxv      vs51,0(r3)

    # r[1]
    xxmrghd   vs33,vs44,vs45
    xxmrghd   vs34,vs36,vs35
    vmsumudm  v19,v1,v2,v0
    stxv      vs51,16(r3)

    # r[2]
    xxmrghd   vs34,vs37,vs36
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v14,v3,v19
    stxv      vs51,32(r3)

    # r[3]
    xxmrghd   vs34,vs36,vs35
    xxmrghd   vs42,vs46,vs47
    xxmrghd   vs43,vs38,vs37
    vmsumudm  v19,v1,v11,v0
    vmsumudm  v19,v10,v2,v19
    stxv      vs51,48(r3)

    # r[4]
    xxmrghd   vs34,vs39,vs38
    xxmrghd   vs43,vs37,vs36
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v10,v11,v19
    vmsumudm  v19,v16,v3,v19
    stxv      vs51,64(r3)

    # r[5]
    xxmrghd   vs34,vs40,vs39
    xxmrghd   vs43,vs38,vs37
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v10,v11,v19
    xxmrghd   vs43,vs36,vs35
    xxmrghd   vs33,vs48,vs49
    vmsumudm  v19,v1,v11,v19
    stxv      vs51,80(r3)

    # r[6]
    xxmrghd   vs33,vs44,vs45
    xxmrghd   vs34,vs41,vs40
    xxmrghd   vs43,vs39,vs38
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v10,v11,v19
    xxmrghd   vs34,vs37,vs36
    xxmrghd   vs33,vs48,vs49
    vmsumudm  v19,v1,v2,v19
    vmsumudm  v19,v18,v3,v19
    stxv      vs51,96(r3)

    # r[7]
    xxmrghd   vs33,vs45,vs46
    xxmrghd   vs34,vs41,vs40
    xxmrghd   vs42,vs47,vs48
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v10,v11,v19
    xxmrghd   vs42,vs37,vs36
    xxmrghd   vs33,vs49,vs50
    vmsumudm  v19,v1,v10,v19
    stxv      vs51,112(r3)

    # r[8]
    xxmrghd   vs33,vs46,vs47
    xxmrghd   vs42,vs48,vs49
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v10,v11,v19
    vmsumudm  v19,v18,v5,v19
    stxv      vs51,128(r3)

    # r[9]
    xxmrghd   vs33,vs47,vs48
    vmsumudm  v19,v1,v2,v0
    xxmrghd   vs33,vs49,vs50
    vmsumudm  v19,v1,v11,v19
    stxv      vs51,144(r3)

    # r[10]
    vmsumudm  v19,v10,v2,v0
    vmsumudm  v19,v18,v7,v19
    stxv      vs51,160(r3)

    # r[11]
    vmsumudm  v19,v1,v2,v0
    stxv      vs51,176(r3)

    # r[12]
    vmsumudm  v19,v18,v9,v0
    stxv      vs51,192(r3)

    blr
.size mult,.-mult


.globl square
.align 5
square:
    xxspltib  v0,0

    # load a
    lxsd      v12,0(r4)
    lxsd      v13,8(r4)
    lxsd      v14,16(r4)
    lxsd      v15,24(r4)
    lxsd      v16,32(r4)
    lxsd      v17,40(r4)
    lxsd      v18,48(r4)

    # shift leftward to get a2
    li        r8,0
    li        r9,1
    mtvsrdd   vs33,r9,r8
    vsld      v3,v12,v1
    vsld      v4,v13,v1
    vsld      v5,v14,v1
    vsld      v6,v15,v1
    vsld      v7,v16,v1
    vsld      v8,v17,v1
    vsld      v9,v18,v1

    # r[0]
    vmsumudm  v19,v12,v12,v0
    stxv      vs51,0(r3)

    # r[1]
    vmsumudm  v19,v12,v4,v0
    stxv      vs51,16(r3)

    # r[2]
    vmsumudm  v19,v12,v5,v0
    vmsumudm  v19,v13,v13,v19
    stxv      vs51,32(r3)

    # r[3]
    xxmrghd   vs33,vs44,vs45
    xxmrghd   vs34,vs38,vs37
    vmsumudm  v19,v1,v2,v0
    stxv      vs51,48(r3)

    # r[4]
    xxmrghd   vs43,vs39,vs38
    vmsumudm  v19,v1,v11,v0
    vmsumudm  v19,v14,v14,v19
    stxv      vs51,64(r3)

    # r[5]
    xxmrghd   vs34,vs40,vs39
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v14,v6,v19
    stxv      vs51,80(r3)

    # r[6]
    xxmrghd   vs34,vs41,vs40
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v14,v7,v19
    vmsumudm  v19,v15,v15,v19
    stxv      vs51,96(r3)

    # r[7]
    xxmrghd   vs42,vs45,vs46
    vmsumudm  v19,v10,v2,v0
    vmsumudm  v19,v15,v7,v19
    stxv      vs51,112(r3)

    # r[8]
    xxmrghd   vs33,vs46,vs47
    vmsumudm  v19,v1,v2,v0
    vmsumudm  v19,v16,v16,v19
    stxv      vs51,128(r3)

    # r[9]
    xxmrghd   vs33,vs47,vs48
    vmsumudm  v19,v1,v2,v0
    stxv      vs51,144(r3)

    # r[10]
    vmsumudm  v19,v16,v9,v0
    vmsumudm  v19,v17,v17,v19
    stxv      vs51,160(r3)

    # r[11]
    vmsumudm  v19,v17,v9,v0
    stxv      vs51,176(r3)

    # r[12]
    vmsumudm  v19,v18,v18,v0
    stxv      vs51,192(r3)

    blr
.size square,.-square


.globl reduce
.align 5
reduce:
    xxspltib  vs32,0

    # load a
    lxv       vs35,0(r4)
    lxv       vs36,16(r4)
    lxv       vs37,32(r4)
    lxv       vs38,48(r4)
    lxv       vs39,64(r4)
    lxv       vs40,80(r4)
    lxv       vs41,96(r4)
    lxv       vs42,112(r4)
    lxv       vs43,128(r4)
    lxv       vs44,144(r4)
    lxv       vs45,160(r4)
    lxv       vs46,176(r4)
    lxv       vs47,192(r4)

    # underflow avoidance constants
    addis     r10,r2,.Lreduce@toc@ha
    addi      r10,r10,.Lreduce@toc@l

    lxv       vs48,0(r10)
    lxv       vs49,16(r10)
    lxv       vs50,32(r10)
    lxv       vs51,48(r10)

    vadduqm   v3,v3,v16
    vadduqm   v4,v4,v17
    vadduqm   v5,v5,v18
    vadduqm   v6,v6,v19
    vadduqm   v7,v7,v19
    vadduqm   v8,v8,v19
    vadduqm   v9,v9,v19

    # xxpermr masks to perform combined clear and shift
    lxv       vs0,64(r10)
    lxv       vs1,80(r10)
    lxv       vs2,96(r10)
    lxv       vs3,112(r10)

    # substitute limbs [7-12]

    # sub acc[12]
    xxspltib  vs48,32
    vmr       v17,v0
    vsro      v16,v15,v16
    xxpermr   vs49,vs47,vs0
    vadduqm   v11,v11,v16
    vadduqm   v10,v10,v17
    xxspltib  vs48,8
    vmr       v17,v0
    vsro      v16,v15,v16
    xxpermr   vs49,vs47,vs1
    vadduqm   v10,v10,v16
    vadduqm   v9,v9,v17
    xxspltib  vs48,16
    vmr       v17,v0
    vsro      v16,v15,v16
    xxpermr   vs49,vs47,vs2
    vsubuqm   v9,v9,v16
    vsubuqm   v8,v8,v17
    xxspltib  vs48,48
    vmr       v17,v0
    vsro      v16,v15,v16
    xxpermr   vs49,vs47,vs3
    vadduqm   v9,v9,v16
    vadduqm   v8,v8,v17

    # sub acc[11]
    xxspltib  vs48,32
    vmr       v17,v0
    vsro      v16,v14,v16
    xxpermr   vs49,vs46,vs0
    vadduqm   v10,v10,v16
    vadduqm   v9,v9,v17
    xxspltib  vs48,8
    vmr       v17,v0
    vsro      v16,v14,v16
    xxpermr   vs49,vs46,vs1
    vadduqm   v9,v9,v16
    vadduqm   v8,v8,v17
    xxspltib  vs48,16
    vmr       v17,v0
    vsro      v16,v14,v16
    xxpermr   vs49,vs46,vs2
    vsubuqm   v8,v8,v16
    vsubuqm   v7,v7,v17
    xxspltib  vs48,48
    vmr       v17,v0
    vsro      v16,v14,v16
    xxpermr   vs49,vs46,vs3
    vadduqm   v8,v8,v16
    vadduqm   v7,v7,v17

    # sub acc[10]
    xxspltib  vs48,32
    vmr       v17,v0
    vsro      v16,v13,v16
    xxpermr   vs49,vs45,vs0
    vadduqm   v9,v9,v16
    vadduqm   v8,v8,v17
    xxspltib  vs48,8
    vmr       v17,v0
    vsro      v16,v13,v16
    xxpermr   vs49,vs45,vs1
    vadduqm   v8,v8,v16
    vadduqm   v7,v7,v17
    xxspltib  vs48,16
    vmr       v17,v0
    vsro      v16,v13,v16
    xxpermr   vs49,vs45,vs2
    vsubuqm   v7,v7,v16
    vsubuqm   v6,v6,v17
    xxspltib  vs48,48
    vmr       v17,v0
    vsro      v16,v13,v16
    xxpermr   vs49,vs45,vs3
    vadduqm   v7,v7,v16
    vadduqm   v6,v6,v17

    # sub acc[9]
    xxspltib  vs48,32
    vmr       v17,v0
    vsro      v16,v12,v16
    xxpermr   vs49,vs44,vs0
    vadduqm   v8,v8,v16
    vadduqm   v7,v7,v17
    xxspltib  vs48,8
    vmr       v17,v0
    vsro      v16,v12,v16
    xxpermr   vs49,vs44,vs1
    vadduqm   v7,v7,v16
    vadduqm   v6,v6,v17
    xxspltib  vs48,16
    vmr       v17,v0
    vsro      v16,v12,v16
    xxpermr   vs49,vs44,vs2
    vsubuqm   v6,v6,v16
    vsubuqm   v5,v5,v17
    xxspltib  vs48,48
    vmr       v17,v0
    vsro      v16,v12,v16
    xxpermr   vs49,vs44,vs3
    vadduqm   v6,v6,v16
    vadduqm   v5,v5,v17

    # sub acc[8]
    xxspltib  vs48,32
    vmr       v17,v0
    vsro      v16,v11,v16
    xxpermr   vs49,vs43,vs0
    vadduqm   v7,v7,v16
    vadduqm   v6,v6,v17
    xxspltib  vs48,8
    vmr       v17,v0
    vsro      v16,v11,v16
    xxpermr   vs49,vs43,vs1
    vadduqm   v6,v6,v16
    vadduqm   v5,v5,v17
    xxspltib  vs48,16
    vmr       v17,v0
    vsro      v16,v11,v16
    xxpermr   vs49,vs43,vs2
    vsubuqm   v5,v5,v16
    vsubuqm   v4,v4,v17
    xxspltib  vs48,48
    vmr       v17,v0
    vsro      v16,v11,v16
    xxpermr   vs49,vs43,vs3
    vadduqm   v5,v5,v16
    vadduqm   v4,v4,v17

    # sub acc[7]
    xxspltib  vs48,32
    vmr       v17,v0
    vsro      v16,v10,v16
    xxpermr   vs49,vs42,vs0
    vadduqm   v6,v6,v16
    vadduqm   v5,v5,v17
    xxspltib  vs48,8
    vmr       v17,v0
    vsro      v16,v10,v16
    xxpermr   vs49,vs42,vs1
    vadduqm   v5,v5,v16
    vadduqm   v4,v4,v17
    xxspltib  vs48,16
    vmr       v17,v0
    vsro      v16,v10,v16
    xxpermr   vs49,vs42,vs2
    vsubuqm   v4,v4,v16
    vsubuqm   v3,v3,v17
    xxspltib  vs48,48
    vmr       v17,v0
    vsro      v16,v10,v16
    xxpermr   vs49,vs42,vs3
    vadduqm   v4,v4,v16
    vadduqm   v3,v3,v17

    # preemptive carry, 4 -> 5 -> 6
    li        r8,-1
    clrldi    r8,r8,8
    mtvsrdd   vs7,0,r8
    xxspltib  vs48,56
    vsro      v17,v7,v16
    xxland    vs39,vs39,vs7
    vadduqm   v8,v8,v17

    vsro      v18,v8,v16
    xxland    vs40,vs40,vs7
    vadduqm   v9,v9,v18

    xxspltib  vs51,48
    vsro      v19,v9,v19
    li        r8,-1
    clrldi    r8,r8,16
    mtvsrdd   vs48,0,r8
    xxland    vs41,vs41,vs48

    # xxpermr masks to perform combined clear and shift
    lxv       vs4,128(r10)
    lxv       vs5,144(r10)
    lxv       vs6,160(r10)

    # substitute high bits of limb 6
    xxspltib  vs48,40
    vmr       v17,v0
    vsro      v16,v19,v16
    xxpermr   vs49,vs51,vs4
    vadduqm   v6,v6,v16
    vadduqm   v5,v5,v17
    xxspltib  vs48,16
    vmr       v17,v0
    vsro      v16,v19,v16
    xxpermr   vs49,vs51,vs5
    vadduqm   v5,v5,v16
    vadduqm   v4,v4,v17
    xxspltib  vs48,24
    vmr       v17,v0
    vsro      v16,v19,v16
    xxpermr   vs49,vs51,vs6
    vsubuqm   v4,v4,v16
    vsubuqm   v3,v3,v17
    vadduqm   v3,v3,v19

    # final carry
    xxspltib  vs48,56

    vsro      v17,v3,v16
    xxland    vs35,vs35,vs7
    vadduqm   v4,v4,v17

    vsro      v17,v4,v16
    xxland    vs36,vs36,vs7
    vadduqm   v5,v5,v17

    vsro      v17,v5,v16
    xxland    vs37,vs37,vs7
    vadduqm   v6,v6,v17

    vsro      v17,v6,v16
    xxland    vs38,vs38,vs7
    vadduqm   v7,v7,v17

    vsro      v17,v7,v16
    xxland    vs39,vs39,vs7
    vadduqm   v8,v8,v17

    vsro      v17,v8,v16
    xxland    vs40,vs40,vs7
    vadduqm   v9,v9,v17

    # pack and store back
    xxmrgld   vs35,vs36,vs35
    stxv      vs35,0(r3)

    xxmrgld   vs37,vs38,vs37
    stxv      vs37,16(r3)

    xxmrgld   vs39,vs40,vs39
    stxv      vs39,32(r3)

    xxmrgld   vs41,vs41,vs32
    stxsd     v9,48(r3)

    blr
.align 4
.Lreduce:
    .octa 0x10000ffffffff0000000000000000000
    .octa 0x0feffffffffffff00000000000000000
    .octa 0x0fffffffeffffff00000000000000000
    .octa 0x0ffffffffffffff00000000000000000

    .octa	0x00000000000000000013121110000000
    .octa	0x00000000000000000010000000000000
    .octa	0x00000000000000000011100000000000
    .octa	0x00000000000000000015141312111000

    .octa	0x00000000000000000014131211100000
    .octa	0x00000000000000000011100000000000
    .octa	0x00000000000000000012111000000000
.size reduce,.-reduce


.globl diff_128
.align 5
diff_128:
    xxspltib  vs32,0

    # load a
    lxv       vs35,0(r3)
    lxv       vs36,16(r3)
    lxv       vs37,32(r3)
    lxv       vs38,48(r3)
    lxv       vs39,64(r3)
    lxv       vs40,80(r3)
    lxv       vs41,96(r3)
    lxv       vs42,112(r3)
    lxv       vs43,128(r3)
    lxv       vs44,144(r3)
    lxv       vs45,160(r3)
    lxv       vs46,176(r3)
    lxv       vs47,192(r3)

    # underflow avoidance constants
    addis     r10,r2,0
    addi      r10,r10,0
    lxv       vs48,0(r10)
    vadduqm   v3,v3,v16

    lxv       vs48,16(r10)
    lxv       vs49,32(r10)
    lxv       vs50,48(r10)
    lxv       vs51,64(r10)

    vadduqm   v4,v4,v16
    vadduqm   v5,v5,v16
    vadduqm   v6,v6,v16
    vadduqm   v7,v7,v16
    vadduqm   v8,v8,v16
    vadduqm   v9,v9,v17
    vadduqm   v10,v10,v18
    vadduqm   v11,v11,v19
    vadduqm   v12,v12,v16
    vadduqm   v13,v13,v16
    vadduqm   v14,v14,v16
    vadduqm   v15,v15,v16

    # r[0]
    lxv       vs48,0(r4)
    vsubuqm   v3,v3,v16

    # r[1]
    lxv       vs49,16(r4)
    vsubuqm   v4,v4,v17

    # r[2]
    lxv       vs50,32(r4)
    vsubuqm   v5,v5,v18

    # r[3]
    lxv       vs51,48(r4)
    vsubuqm   v6,v6,v19

    # r[4]
    lxv       vs48,64(r4)
    vsubuqm   v7,v7,v16

    # r[5]
    lxv       vs49,80(r4)
    vsubuqm   v8,v8,v17

    # r[6]
    lxv       vs50,96(r4)
    vsubuqm   v9,v9,v18

    # r[7]
    lxv       vs51,112(r4)
    vsubuqm   v10,v10,v19

    # r[8]
    lxv       vs48,128(r4)
    vsubuqm   v11,v11,v16

    # r[9]
    lxv       vs49,144(r4)
    vsubuqm   v12,v12,v17

    # r[10]
    lxv       vs50,160(r4)
    vsubuqm   v13,v13,v18

    # r[11]
    lxv       vs51,176(r4)
    vsubuqm   v14,v14,v19

    # r[12]
    lxv       vs48,192(r4)
    vsubuqm   v15,v15,v16

    stxv      vs35,0(r3)
    stxv      vs36,16(r3)
    stxv      vs37,32(r3)
    stxv      vs38,48(r3)
    stxv      vs39,64(r3)
    stxv      vs40,80(r3)
    stxv      vs41,96(r3)
    stxv      vs42,112(r3)
    stxv      vs43,128(r3)
    stxv      vs44,144(r3)
    stxv      vs45,160(r3)
    stxv      vs46,176(r3)
    stxv      vs47,192(r3)

    blr
.align 4
.Ladd_128:
    .octa   0x80000000000000000000000000000000
    .octa   0x7fffffffffffff800000000000000000
    .octa   0x80007fffffff7f800000000000000000
    .octa   0x7f7fffffffffff800000000000000000
    .octa   0x7fffffff7fffff800000000000000000
.size diff_128,.-diff_128


.globl diff_mixed
.align 5
diff_mixed:
    xxspltib  vs32,0

    # load a
    lxv       vs35,0(r3)
    lxv       vs36,16(r3)
    lxv       vs37,32(r3)
    lxv       vs38,48(r3)
    lxv       vs39,64(r3)
    lxv       vs40,80(r3)
    lxv       vs41,96(r3)

    # underflow avoidance constants
    addis     r10,r2,.Ladd_mixed@toc@ha
    addi      r10,r10,.Ladd_mixed@toc@l
    lxv       vs48,0(r10)
    lxv       vs49,16(r10)
    lxv       vs50,32(r10)
    lxv       vs51,48(r10)

    vadduqm   v3,v3,v16
    vadduqm   v4,v4,v17
    vadduqm   v5,v5,v18
    vadduqm   v6,v6,v19
    vadduqm   v7,v7,v19
    vadduqm   v8,v8,v19
    vadduqm   v9,v9,v19

    # r[0]
    ld        r8,0(r4)
    mtvsrdd   vs48,0,r8
    vsubuqm   v3,v3,v16

    # r[1]
    ld        r8,8(r4)
    mtvsrdd   vs48,0,r8
    vsubuqm   v4,v4,v16

    # r[2]
    ld        r8,16(r4)
    mtvsrdd   vs48,0,r8
    vsubuqm   v5,v5,v16

    # r[3]
    ld        r8,24(r4)
    mtvsrdd   vs48,0,r8
    vsubuqm   v6,v6,v16

    # r[4]
    ld        r8,32(r4)
    mtvsrdd   vs48,0,r8
    vsubuqm   v7,v7,v16

    # r[5]
    ld        r8,40(r4)
    mtvsrdd   vs48,0,r8
    vsubuqm   v8,v8,v16

    # r[6]
    ld        r8,48(r4)
    mtvsrdd   vs48,0,r8
    vsubuqm   v9,v9,v16

    stxv      vs35,0(r3)
    stxv      vs36,16(r3)
    stxv      vs37,32(r3)
    stxv      vs38,48(r3)
    stxv      vs39,64(r3)
    stxv      vs40,80(r3)
    stxv      vs41,96(r3)

    blr
.align 4
.Ladd_mixed:
    .octa   0x00000000000000010000ffffffff0000
    .octa   0x0000000000000000feffffffffffff00
    .octa   0x0000000000000000fffffffeffffff00
    .octa   0x0000000000000000ffffffffffffff00
.size diff_mixed,.-diff_mixed
