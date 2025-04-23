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
Usage: ./keccak <function_name> <input_message>

# example usage
$ ./keccak sha3-256 "abc"

# Read from file
Usage: ./test <func_name> <in_len> <in_file> <out_file>

# example usage
$ ./test shake-128 0 tests/SHAKE128ShortMsg.txt out.txt
```

## Testing
Output testing requires that Python 3 is present on your system. The [NIST](https://csrc.nist.gov/Projects/cryptographic-algorithm-validation-program/Secure-Hashing) test vector files are available in .txt format within the 'tests' directory.

Make sure you have both the keccak and test binaries ready before running the Python script.

## Usage
To run the tests, navigate to the `tests` directory and use the following commands:
#### Testing with Short and Long Vectors
```bash
$ python3 test-long-short-vectors.py <function-name>
```
#### Testing with SHA3 Monte Carlo Vectors
```bash
$ python3 test-sha3-monte-vectors.py <function-name>
```
#### Testing with SHAKE Monte Carlo Vectors
```bash
$ python3 test-shake-monte-vectors.py
```
#### Testing with SHAKE Variable Output Vectors
```bash
$ python3 test-shake-variable-out.py <function-name>
```
#### Run All Tests
```bash
$ chmod +x test-all.sh
$ ./test-all.sh
```

