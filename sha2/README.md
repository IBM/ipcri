# SHA2

## Foreword
This directory contains a standalone, rudimentary implementation of SHA2.
This is not intended to be used as-is; rather, the assembly implementations should be taken as reference and included in more robust libraries.
The simple C wrappings are for testing purposes only.

For API usage, see USAGE.md

There are also binaries provided that run the SHA2 compress function in a loop.
These are intended for performance analysis, and do not actually yield any useful output.

## Building

```
# Build everything
$ make 

# Build only the hashing binaries
$ make bins

# Build only the looping binaries
$ make loops

# Build and run the throughput benchmark utilities
$ make bench

# Clean up compiled/generated files
$ make clean
```

## Building OpenSSL

For comparing against OpenSSL, the Makefile provides some targets to build binaries using OpenSSL's assembly.
Either clone/copy/symlink an OpenSSL source to `openssl`, or provide a path to one by setting the `OPENSSL_PATH` variable.
This will build the same hash, bench, and loop binaries.

```
# Build with OpenSSL copied to <ipcri root>/sha2/openssl
$ make

# Build with OpenSSL cloned from somewhere else
$ make OPENSSL_PATH=~/path/to/openssl
```


### Cross-Compiling

To cross-compile, set the `CROSS` variable to the toolchain prefix.
A toolchain can be acquired from [here](https://toolchains.bootlin.com).
Select `powerpc64le` or `powerpc` as the arch, and `glibc` as the libc.

```
# Assume the toolchain acquired from bootlin has been extracted to ~/cross/powerpc64le/
make CROSS=~/cross/powerpc64le/bin/powerpc64le-linux-
```

Note that the `check` and `bench` targets will need extra configuring to make work.
See [Cross-Testing](#cross-testing) for how to run tests on non-native hardware.
Benchmarking on non-native hardware is useless, however.

A helper Makefile target has been provided to fetch the toolchains, but may not be reliable if URLs change:
```
# Download both powerpc64 and powerpc64le cross-toolchains
make toolchain

# Download just one toolchain
make cross/powerpc64
make cross/powerpc64le
```

## Testing

The simple test suite uses the python library `unitttest`, and expects a relatively recent version of Python 3 (probably at least 3.6).

```
# Run all the tests
make check

# Run all tests, don't stop if one fails
IGNORE_FAIL=1 make check

# Run a single test with the -check suffix added to the name
# Examples below use the "asm/sha2-naive-attempt.asm" implementation
make sha2-naive-attempt-check

# Run a single test, then also run a single benchmark
make sha2-naive-attempt-check sha2-naive-attempt-bench
```

### Test Vectors

The test vectors used in the testing framework have been sourced from NIST [here](https://csrc.nist.gov/Projects/cryptographic-algorithm-validation-program/Secure-Hashing) in their original format.

### Cross-Testing

Test cases can be run on non-native hardware via `qemu-user-static`.
Major distros may have this packaged:

Fedora:
```
$ dnf install qemu-user-static-ppc
```

Ubuntu:
```
# ppc64el only
$ apt install qemu-user-static
```

A path to a `qemu-user-static` bin may also be provided to make via the `QEMU_PATH` variable, if pointing to a locally built version:
```
# This will use the little-endian emulator, ensure you point to qemu-ppc64-static if building for big-endian
$ QEMU_PATH=/path/to/qemu-ppc64le-static make check
  
```

Test cases can also be run targeting specific hardware by setting the `CPU_TYPE` variable to `power8`, `power9`, or `power10`.
Note that this will still infer `ENDIAN`/`ARCH` from your `CROSS` prefix, so you may need to set that manually.
This will automatically assume `qemu-user-static` binaries are available in `$PATH`; be sure to set `QEMU_PATH` manually if this is not the case.
```
# Infers ENDIAN=LITTLE from CROSS, assumes qemu-ppc64le-static is in $PATH
$ CPU_TYPE=power8 CROSS=./cross/powerpc64le/bin/powerpc64le-linux- make check

# Manually set ENDIAN, assumes qemu-ppc64-static is in $PATH
$ CPU_TYPE=power8 ENDIAN=BIG make check

# Manually set ENDIAN and QEMU_PATH
$ CPU_TYPE=power8 ENDIAN=BIG QEMU_PATH=/path/to/qemu-ppc64-static make check
```

#### Cross-Testing Helpers

Some helpers are provided to simplify testing on x86.
These still depend on qemu-user-static as marked above, and will only test the default CPU_TYPE.

```
# Download toolchains (if needed) and test both big and little endian
make ci

# Rebuild and test only one endian
make little-check
make big-check
```
