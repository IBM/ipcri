# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024-2025 IBM Corp. All rights reserved
# Authored by Nimet Ozkan <nozkan@linux.ibm.com>
#

CC = gcc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wvla -Wpointer-arith -O3 -flto -fomit-frame-pointer
LIBS = -L. -lkeccak

SRC = src/keccak.c src/bench.c ../common/util.c 
SRC_KECCAK = main.c $(SRC)
SRC_TEST = test.c $(SRC)

KECCAK = keccak
TEST = test
SHARED_LIB = libkeccak.so

all: $(KECCAK) $(TEST) $(SHARED_LIB)

$(KECCAK): $(SRC_KECCAK)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST): $(SRC_TEST)
	$(CC) -o $@ $^

$(SHARED_LIB): $(SRC)
	$(CC) -shared -o $@ -fPIC $^

clean:
	rm -f $(KECCAK) $(TEST) $(SHARED_LIB)

.PHONY: all clean
