# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024- IBM Corp. All rights reserved
# Authored by Eric Richter <erichte@linux.ibm.com>
#
CC ?= gcc
CC := $(CROSS)$(CC)

# convert powerpc64 -> ppc64 in the dumbest way possible because there's no consistency anywhere
ARCH ?= $(subst ower,,$(shell $(CC) -dumpmachine | cut -d "-" -f 1))
ifeq ($(ARCH),)
$(warning Unable to get compiler architecture, make sure CROSS or CC is set correctly)
endif

# Set endianness, reject non-power compiliers
ifeq ($(ARCH),ppc64)
ENDIAN=BIG
else ifeq ($(ARCH),ppc64le)
ENDIAN=LITTLE
else
$(warning $(CC) appears to be building for $(ARCH), consider setting CROSS to a valid powerpc(le) toolchain prefix)
# Arbitarily define this, as it causes issues with the -D flag if empty
ENDIAN=LITTLE
endif

OPENSSL_PATH ?= openssl
# Arbitrarily check for the ppc-xlate file, since it will be needed anyway
OSSL_ENABLED = $(shell test -f $(OPENSSL_PATH)/crypto/perlasm/ppc-xlate.pl && echo 1)

ifneq ($(OSSL_ENABLED),)
OSSL_BINS=sha256-ossl sha512-ossl
OSSL_BINS:=$(addprefix bin/,$(OSSL_BINS))
else
OSSL_BINS=
endif

P8_BINS=$(patsubst asm/%.S,bin/%,$(wildcard asm/*.S)) $(OSSL_BINS)

CFLAGS ?= -g

# Build static if we are cross compiling, so that the tests can be run via qemu-user-static
ifneq ($(CROSS),)
CFLAGS += -static
endif


all: bins loops benches

bins: $(P8_BINS)
loops: $(patsubst bin/%,bin/loops/%,$(P8_BINS))
benches: $(patsubst bin/%,bin/bench/%,$(P8_BINS))

##### Assembly macroizers
# Apply C preprocessor to "pure" assembly
gen/%.s: asm/%.S
	@mkdir -p gen
	cpp -I. -D$(ENDIAN) -o $@ $<

### OpenSSL Perl Hacks
ifneq ($(OSSL_ENABLED),)
# Apply clean-roomed perlasm ABI fixes:
#  - Rename symbol to ipcri_shaXXX_func
#  - Change input word register to r6
#  - Change previously used r6 to r4, the unused-by-OpenSSL K input register
gen/sha%p8-ppc.pl: $(OPENSSL_PATH)/crypto/sha/asm/sha512p8-ppc.pl
	@mkdir -p gen
	@cp $(OPENSSL_PATH)/crypto/perlasm/ppc-xlate.pl gen/
	cat $^ | sed -e 's/sha.*_block_p8/ipcri_sha$*_func/g' | sed -e 's/inp="r4"/inp="r6"/' | sed -e 's/Tbl="r6"/Tbl="r4"/' > $@

# Execute OpenSSL Perl Assembly
# Usage: perl sha512p8-ppc.pl <flavor, containing 64 or 64le> <output file, containing 256 or 512>
# Example: perl sha512p8-ppc.pl powerpc64le gen/sha256-ossl.s
.PRECIOUS: gen/sha%-ossl.s
gen/sha%-ossl.s: gen/sha%p8-ppc.pl
	perl $^ $(ARCH) $@

endif

##### Building

## SHA256

.PRECIOUS: gen/%.s
bin/sha256-%: src/sha2.c gen/sha256-%.s
	@mkdir -p bin	
	$(CC) -o $@ $(CFLAGS) $^

.PRECIOUS: gen/%.s
bin/loops/sha256-%: src/sha2-loop.c gen/sha256-%.s
	@mkdir -p bin/loops/
	$(CC) -o $@ $(CFLAGS) $^

.PRECIOUS: gen/%.s
bin/bench/sha256-%: src/sha2-bench.c gen/sha256-%.s
	@mkdir -p bin/bench/
	$(CC) -o $@ $(CFLAGS) $^

## SHA512

.PRECIOUS: gen/%.s
bin/sha512-%: src/sha2.c gen/sha512-%.s
	@mkdir -p bin
	$(CC) -o $@ $(CFLAGS) -DSHA512 $^

.PRECIOUS: gen/%.s
bin/loops/sha512-%: src/sha2-loop.c gen/sha512-%.s
	@mkdir -p bin/loops/
	$(CC) -o $@ $(CFLAGS) -DSHA512 $^

.PRECIOUS: gen/%.s
bin/bench/sha512-%: src/sha2-bench.c gen/sha512-%.s
	@mkdir -p bin/bench/
	$(CC) -o $@ $(CFLAGS) -DSHA512 $^

##### Misc Helpers

%-bench: bin/bench/%
	$^
	@echo

bench: $(patsubst bin/%,%-bench,$(P8_BINS))

%-check: bin/%
	ENDIAN=$(ENDIAN) make -C test/ $(patsubst bin/%,%,$^)-check

check: $(patsubst bin/%,%-check,$(P8_BINS))

clean:
	rm -rf sha256 gen/ bin/

##### Cross Compilation Toolchain Magic™ for x86_64
ifeq ($(shell uname -m),x86_64)

TOOLCHAIN_URL=https://toolchains.bootlin.com/downloads/releases/toolchains/ARCH-power8/tarballs/ARCH-power8--glibc--stable-2022.08-1.tar.bz2
CROSS_PREFIX=$(shell pwd)/cross/$*/bin/$*-linux-

cross/%:
	@mkdir -p $@
	@curl $(subst ARCH,$*,$(TOOLCHAIN_URL)) | tar --strip-components=1 -C $@ -xjf -
	@echo To use, export CROSS=$(CROSS_PREFIX)

toolchains: cross/powerpc64 cross/powerpc64le

# Remove any leftover qemu-user-static core dumps from tests that might fail due to SIGILL, etc
coreclean:
	rm -f qemu*.core test/qemu*.core

deepclean: clean coreclean
	rm -rf cross/

ci-%: cross/%
	@make clean
	@echo Testing $*...
	@make check CROSS=$(CROSS_PREFIX)

ci: ci-powerpc64 ci-powerpc64le
big-check: ci-powerpc64
little-check: ci-powerpc64le

endif
