## Build
```bash
# build keccak binary
$ make
# build test binary
$ make test
# clean binaries
$ make clean
```
## Usage
```bash
# Read from command line
Usage: ./keccak <function_name> <input_message1> <input_message2>

# example usage
$ ./keccak sha3-256 "abc" "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"

# Read from file
Usage: ./test <func_name> <in_len> <in_file> <out_file>

# example usage
$ ./test shake-128 0 tests/SHAKE128ShortMsg.txt out.txt
```

## Testing
Output testing requires that Python 3 is present on your system. The [NIST](https://csrc.nist.gov/Projects/cryptographic-algorithm-validation-program/Secure-Hashing) test vector files are available in .txt format within the 'tests' directory.

Make sure you have both the keccak and test binaries ready before running the Python script.
```bash
# usage
$ cd tests
$ python3 run.py <func_name>
```
