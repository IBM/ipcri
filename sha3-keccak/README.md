# SHA3-KECCAK  
This directory contains the SHA3-Keccak algorithm code, developed in C with PowerPC intrinsics. Run it on a Power10 machine for optimal performance. It supports both single and multi-buffer options.

## Requirements
- **PowerPC 10 architecture**
- **GCC version 13 or higher**
- **RHEL version 10 or higher**

## Supported Functions
- **SHA3-224**
- **SHA3-256**
- **SHA3-384**
- **SHA3-512**
- **SHAKE-128**
- **SHAKE-256**

## Build
To build the binaries, use the following commands:

### Build both single and multi-buffer binaries:
```bash
$ make all
# clean binaries
$ make clean
```
### Build only the single-buffer binary:
```bash
$ make single
# Alternatively, navigate to the singlebuffer directory and run:
$ make
```
### Build only the multi-buffer binary:
```bash
$ make multi
# Alternatively, navigate to the multibuffer directory and run:
$ make
```

## Usage
### Run single-buffer:
```bash
Usage: ./sha3-keccak -SINGLE <alg-name> <input>

# example usage
$ ./sha3-keccak sha3-256 "abc"
```
### Run multi-buffer:
```bash
Usage: ./sha3-keccak -MULTI <alg-name> <input1> <input2>

# example usage
$ ./sha3-keccak sha3-256 "abc" "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
```

