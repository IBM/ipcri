# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024-2025 IBM Corp. All rights reserved
# Authored by Nimet Ozkan <nozkan@linux.ibm.com>
#

singlebuff := ./singlebuffer
multibuff := ./multibuffer
run := ./sha3-keccak

.all := all

targets := single multi

help:
	@echo "Usage:"
	@echo "  make all            Builds both singlebuffer and multibuffer binaries"
	@echo "  make single         Builds singlebuffer"
	@echo "  make multi          Builds multi-buffer"
	@echo "  make clean          Cleans the build directories"

.PHONY: all
all: $(targets)

.PHONY: single
single:
	@echo "Building single-buffer..."
	$(MAKE) -C $(singlebuff)

.PHONY: multi
multi:
	@echo "Building multi-buffer..."
	$(MAKE) -C $(multibuff)

.PHONY: clean
clean:
	@echo "Cleaning single-buffer..."
	$(MAKE) -C $(singlebuff) clean
	@echo "Cleaning multi-buffer..."
	$(MAKE) -C $(multibuff) clean

%.o: %.c
	$(CC) -c $< -o $@
