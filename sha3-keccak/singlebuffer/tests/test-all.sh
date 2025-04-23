#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024-2025 IBM Corp. All rights reserved
# Authored by Nimet Ozkan <nozkan@linux.ibm.com>
#
SHA3_ARGS=("sha3-256" "sha3-224" "sha3-384" "sha3-512")
SHAKE_ARGS=("shake-128" "shake-256")
LONGSHORT_ARGS=("${SHA3_ARGS[@]}" "${SHAKE_ARGS[@]}")

# run monte carlo tests for sha3
for arg in "${SHA3_ARGS[@]}"; do
    echo "Running: Monte Carlo Test Vectors for $arg"
    python3 test-sha3-monte-vectors.py "$arg"
done

# run variable output test vectors for shake128 and shake256
for arg in "${SHAKE_ARGS[@]}"; do
    echo "Running: Variable Output Test Vectors for $arg"
    python3 test-shake-variable-vectors.py "$arg"
    echo "Running: Monte Carlo Test Vectors for $arg"
    python3 test-shake-monte-vectors.py "$arg"
done

# run test short and long test vectors both sha3 and shake
for arg in "${LONGSHORT_ARGS[@]}"; do
    echo "Running: Short & Long Test Vectors for $arg"
    python3 test-long-short-vectors.py "$arg"
done
