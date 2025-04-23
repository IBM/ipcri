# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024-2025 IBM Corp. All rights reserved
# Authored by Nimet Ozkan <nozkan@linux.ibm.com>
#

import argparse
import re
import base64
import ctypes

def set_shared_lib() -> ctypes.CDLL:
    lib = ctypes.CDLL('../libkeccak.so')
    lib.sha3_call.argtypes = [ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int, ctypes.POINTER(ctypes.c_uint8)]
    return lib

def tobytes(bits) -> int:
    return (bits // 8)

def sha(lib, bits, message) -> bytes:
    msgsz = len(message)    
    msg = (ctypes.c_uint8 * msgsz)(*message)
    md = (ctypes.c_uint8 * (tobytes(bits)))()  
    lib.sha3_call(bits, msg, msgsz, md)  
    return bytes(md)  

def output(x: bytes) -> bytes:
    return (base64.b16encode(x).decode('utf-8').lower())

def read_monte_seed(filename) -> str:
    with open(filename, 'r') as file:
        seed = file.readline().split("=")[1].strip()
    return(seed)

def read_outputs_from_file(filename) -> list: 
    with open(filename, 'r') as file:
        vectors = []
        for line in file:
            vector = {}
            if '=' in line:
                key, value = line.split(' = ', 1)
                key = key.strip()
                value = value.strip()
                if key == 'COUNT':
                    vector[key] = int(value)
                else:
                    vector[key] = value
                vectors.append(vector)
        return (vectors)

def sha3_monte_carlo(lib, seed, bits) -> list:
    md = [seed]  
    out = []
    for j in range(100):  
        for i in range(1, 1001):  
            message = md[-1]  
            md.append(sha(lib, bits, message))

        md[0] = md[-1]  
        out.append(output(md[0]))
    return out

def compare_outputs(dicts, md_list):
    for index, item in enumerate(dicts):
        if 'COUNT' in item:
            count = item['COUNT']
        elif 'MD' in item:
            md = item['MD']
            if md == md_list[count]:
                print(f'CASE {count} PASSED!')
            else:
                print(f'CASE {count} failed!')

def process(filename, bits):
    lib = set_shared_lib()
    seed = read_monte_seed(filename)
    sha3_call_output = sha3_monte_carlo(lib, base64.b16decode(seed, casefold=True), bits)
    expected = read_outputs_from_file(filename)
    compare_outputs(expected, sha3_call_output)

def main():
    parser = argparse.ArgumentParser(description="Monte Carlo tests for SHA3.")
    parser.add_argument("mode", help="SHA3 mode (e.g., sha3-256, sha3-384, sha3-512, sha3-224)")

    args = parser.parse_args()
    match = re.match(r'sha3-(\d+)', args.mode.strip().lower())

    if match:
        bits = int(match.group(1))  
        filename = f"../../data/SHA3_{bits}Monte.txt"
    else:
        print("Invalid input. Please enter a valid SHA3 variant.")
        return

    process(filename, bits)

if __name__ == "__main__":
    main()

