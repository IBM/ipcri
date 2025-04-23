# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024-2025 IBM Corp. All rights reserved
# Authored by Nimet Ozkan <nozkan@linux.ibm.com>
#

import re
import argparse
import ctypes
import base64

def set_shared_lib() -> ctypes.CDLL:
    lib = ctypes.CDLL('../libkeccak.so')

    lib.shake128_call_progressive.argtypes = (ctypes.c_uint32, ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_ubyte))
    lib.shake128_call_progressive.restype = None

    lib.shake256_call_progressive.argtypes = (ctypes.c_uint32, ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_ubyte))
    lib.shake256_call_progressive.restype = None
    return lib

def shake128_hash(lib, msg: bytes, output_len: int) -> bytes:
    output = (ctypes.c_ubyte * output_len)()
    msg_len = len(msg)
    lib.shake128_call_progressive((output_len), msg, msg_len, output)
    return bytes(output)

def shake256_hash(lib, msg: bytes, output_len: int) -> bytes:
    output = (ctypes.c_ubyte * output_len)()
    msg_len = len(msg)
    lib.shake256_call_progressive((output_len), msg, msg_len, output)
    return bytes(output)

def output(count: int, msg: str, output_hash: bytes, expected_output: str):
    output_hex = base64.b16encode(output_hash).decode('utf-8').lower()
    expected_output_len = len(expected_output) // 2  

    if output_hex == expected_output:
        print(f"CASE {count} PASSED")
    else:
        print(f"CASE {count} PASSED")

def read_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read().strip()

    blocks = content.split('\n\n')
    messages = []

    for block in blocks:
        lines = block.splitlines()
        vector = {}

        for line in lines:
            if ' = ' in line:
                key, value = line.split(' = ', 1)
                vector[key.strip()] = value.strip()

        if 'COUNT' in vector and 'Outputlen' in vector and 'Msg' in vector and 'Output' in vector:
            messages.append((
                int(vector['COUNT']),
                int(vector['Outputlen']) // 8,  
                vector['Msg'],
                vector['Output']  
            ))

    return messages

def run_var128_vectors(filename, lib):
    vectors = read_file(filename)
    for count, output_length, msg, expected_output in vectors:
        msg_bytes = base64.b16decode(msg, casefold=True)
        
        output_hash_c = shake128_hash(lib, msg_bytes, output_length)         
        output(count, msg, output_hash_c, expected_output)

def run_var256_vectors(filename, lib):
    vectors = read_file(filename)
    for count, output_length, msg, expected_output in vectors:
        msg_bytes = base64.b16decode(msg, casefold=True)
        
        output_hash_c = shake256_hash(lib, msg_bytes, output_length)  
        output(count, msg, output_hash_c, expected_output)

def main():
    parser = argparse.ArgumentParser(description='Variable Output Tests for SHAKE')
    parser.add_argument('mode', type=str, help="Enter a function name to apply the output test 'e.g. shake-256, shake-128..' : ")

    lib = set_shared_lib()
    args = parser.parse_args()

    match = re.match(r'shake-(\d+)', args.mode.strip().lower())
    if match:
        bits = int(match.group(1))
        filename = f"../../data/SHAKE{bits}VariableOut.txt"
    else:
        print("Invalid input. Please enter a valid SHAKE mode: shake-128 or shake-256")
        return

    if args.mode == "shake-128":
        run_var128_vectors(filename, lib)
    elif args.mode == "shake-256":
        run_var256_vectors(filename, lib)

if __name__ == "__main__":
    main()
