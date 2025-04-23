# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024-2025 IBM Corp. All rights reserved
# Authored by Nimet Ozkan <nozkan@linux.ibm.com>
#

from pathlib import Path
import os
import shutil
import argparse
import subprocess

'''
USAGE:
  gcc -o test test.c util.c bench.c keccak.c
  python run.py <call-name> 
'''

read = "rb"
write = "wb"
class InvalidFunctionNameGivenError(Exception):
    pass

def load_nist_file(mode):
    nist_files = {
        "SHA3-224":  [f'../../data/{filename}' for filename in ['SHA3_224ShortMsg.txt', 'SHA3_224LongMsg.txt']],
        "SHA3-256":  [f'../../data/{filename}' for filename in ['SHA3_256ShortMsg.txt', 'SHA3_256LongMsg.txt']],
        "SHA3-384":  [f'../../data/{filename}' for filename in ['SHA3_384ShortMsg.txt', 'SHA3_384LongMsg.txt']],
        "SHA3-512":  [f'../../data/{filename}' for filename in ['SHA3_512ShortMsg.txt', 'SHA3_512LongMsg.txt']],
        "SHAKE-128": [f'../../data/{filename}' for filename in ['SHAKE128ShortMsg.txt', 'SHAKE128LongMsg.txt']],
        "SHAKE-256": [f'../../data/{filename}' for filename in ['SHAKE256ShortMsg.txt', 'SHAKE256LongMsg.txt']],
    }.get(mode, [])
    if nist_files:
        return nist_files
    else:
        raise InvalidFunctionNameGivenError("Invalid function name!")

def rm_tmp_dirs():
    dirpath = os.getcwd()
    paths = []
    
    for path in Path(dirpath).iterdir():
        if path.is_dir() and path.name != "nist_vectors":
            try:
                shutil.rmtree(path)
            except PermissionError:
                paths.append(path)

    return paths

def write_file(loc, data):
    try:
        with open(loc, write) as file:
            file.write(data)
    except FileNotFoundError:
        print(loc + "not found.")

def read_vectors(mode):
    vector_files = load_nist_file(mode)

    Len = 0
    Msg = ""
    MD = ""
    v = 0

    for curr_file in vector_files:
        try:
            with open(curr_file, "r") as file:
                print(f"Running: {curr_file}:")
                vectors = file.read().strip().split('\n\n')
                for inx, vector in enumerate(vectors, start=1):
                    lines = vector.split('\n')

                    Len = int("".join(filter(str.isdigit, lines[0].split(' = ')[1].strip()))) // 8
                    Msg = bytes.fromhex("".join(l for l in lines[1].split(' = ')[1].strip() if l.isdigit() or l.lower() in "abcdef"))
                    MD = bytes.fromhex("".join(lines[2].split(' = ')[1].strip().split()))

                    vector_dir = f"vector{v}"
                    os.mkdir(vector_dir)
                    
                    write_file(f"{vector_dir}/msg.txt", Msg)
                    write_file(f"{vector_dir}/md.txt", MD)

                    ret = subprocess.check_output(
                        [
                            "../test",
                            mode,
                            str(Len),
                            f"{vector_dir}/msg.txt",
                            f"{vector_dir}/mdret.txt",
                        ],
                        stderr=subprocess.STDOUT,
                    )
                    
                    write_file(f"{vector_dir}/out.txt", ret)

                    try:
                        with open(f"{vector_dir}/md.txt", read) as md_file, open(
                            f"{vector_dir}/mdret.txt", "rb"
                        ) as md_out_file:
                             print(f"CASE {v}: failed") if md_file.read() != md_out_file.read() else print(f"CASE {v}: PASSED")
                        v += 1
                    except FileNotFoundError:
                        print(f"{vector_dir}/md.txt not found.")

        except FileNotFoundError:
             print(f"File {curr_file} not found.")

def main():
    parser = argparse.ArgumentParser(description='User input for Keccak mode')
    parser.add_argument('mode', type=str, help="Enter a function name to apply the output test 'e.g. sha3-256' : ")
    args = parser.parse_args()

    read_vectors((args.mode).strip().upper())
    rm_tmp_dirs()


if __name__ == "__main__":
    main()

