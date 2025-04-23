# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024-2025 IBM Corp. All rights reserved
# Authored by Nimet Ozkan <nozkan@linux.ibm.com>
#
# https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Algorithm-Validation-Program/documents/sha3/sha3vs.pdf#page=26

import ctypes
import warnings as sad

lib = ctypes.CDLL('../libkeccak.so')
lib.shake128_call.argtypes = (ctypes.c_uint32, ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_ubyte))
lib.shake128_call.restype = None

MIN_OUT_LEN_BITS = 128
MAX_OUT_LEN_BITS = 1120
MIN_OUT_LEN_BYTES = MIN_OUT_LEN_BITS // 8
MAX_OUT_LEN_BYTES = MAX_OUT_LEN_BITS // 8

file_msg = bytes.fromhex("c8b310cb97efa3855434998fa81c7674")

def shake128(msg: bytes, output_len: int) -> bytes:
    output = (ctypes.c_ubyte * output_len)()
    msg_len = len(msg)
    lib.shake128_call((output_len*8//2), msg, msg_len, output)
    return bytes(output)

def generate_monte(file_msg:str, output_len_bits:int, iterations=100) -> list:
    results = []
    msg = file_msg

    for j in range(iterations):
        for i in range(1, 1001):
            output = shake128(msg, output_len_bits // 8)

            msg = output[:16]

            if i == 1000:
                results.append((output.hex()))

            rightmost_output_bits = int.from_bytes(output[-2:], byteorder='big')
            the_range = MAX_OUT_LEN_BYTES - MIN_OUT_LEN_BYTES + 1
            output_len_bits = MIN_OUT_LEN_BITS + ((rightmost_output_bits % the_range) * 8)

    return results

def read_vectors(file_path: str) -> list:
    with open(file_path, 'r') as file:
        content = file.read().strip()

    blocks = content.split('\n\n')
    vectors = []

    for block in blocks:
        lines = block.splitlines()
        vector = {}

        for line in lines:
            if ' = ' in line:
                key, value = line.split(' = ', 1)
                vector[key.strip()] = value.strip()

        if 'COUNT' in vector and 'Outputlen' in vector and 'Output' in vector:
            vectors.append((
                int(vector['COUNT']),
                vector['Output']
            ))

    return vectors

def compare_outputs(from_file:list, from_shake_call:list):
    for index, (cnt, md) in enumerate(from_file):
        if index < len(from_shake_call):
            if md == from_shake_call[index]:
                print(f"CASE {cnt} PASSED")
            else:
                print(f"CASE {cnt} FAILED")
        else:
            print(f"CASE {cnt} FAILED (No corresponding string in the second list)")

outputs = generate_monte(file_msg, MAX_OUT_LEN_BITS)
vectors = read_vectors("../../data/SHAKE128Monte.txt")

compare_outputs(vectors, outputs)
sad.warn("No support for shake256 yet", UserWarning)

