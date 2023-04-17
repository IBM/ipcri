# API Usage

## Preparation & Notes

Each function provided in this directory computes a single compress step in the larger SHA2 algorithm.

The wrapping code to these functions must:
 - supply a precomputed K table[1]
 - apply padding to the input buffer as required[2]
 - ensure the input buffer has enough contiguous blocks of input data to ingest if compressing more than one block at a time.

Compressing multiple blocks may yield better performance, as the assembly code can skip excessive loading and storing from the stack for each block.
The input data **must** be one contiguous buffer of the requested number of blocks to process.
Depending on how this is integrated, it may not be possible to store a contiguous buffer; in this case the number of blocks to process should be 1.

See `sha2.c` for a very rudimentary C implementation using these functions.

[1] FIPS 180-4 sections 4.2.2 and 4.2.3.
[2] FIPS 180-4 section 5.1
https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

## SHA256

`void ipcri_sha256_func(uint32_t state[8], uint32_t K_table[64], size_t num, uint8_t *msg)`

- `uint32_t state[8]` - Current state, 32-bit values A,B,C,D,E,F,G,H.
- `uint32_t K_table[64]` - Pointer to the precomputed table "K" for SHA256.
- `size_t num` - Number of blocks to process. `*msg` must point to a region of memory containing at least `num` contiguous input blocks to be hashed.
- `uint8_t *msg` - Pointer to at least `num` number of input data blocks to be hashed. Input is expected to be pre-padded, and thus contain a multiple of the block size.

For SHA256, one input block is 16 32-bit words, therefore 64 bytes.

## SHA512

`void ipcri_sha256_func(uint64_t state[8], uint64_t K_table[64], size_t num, uint8_t *msg)`

- `uint64_t state[8]` - Current state, 64-bit values A,B,C,D,E,F,G,H.
- `uint64_t K_table[80]` - Pointer to the precomputed table "K" for SHA512.
- `size_t num` - Number of blocks to process. `*msg` must point to a region of memory containing at least `num` contiguous input blocks to be hashed.
- `uint8_t *msg` - Pointer to at least `num` number of input data blocks to be hashed. Input is expected to be pre-padded, and thus contain a multiple of the block size.

For SHA512, one input block is 16 64-bit words, therefore 128 bytes.
