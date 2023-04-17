# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024- IBM Corp. All rights reserved
# Authored by Eric Richter <erichte@linux.ibm.com>
#
BINS=$(wildcard ../bin/sha*)

ifeq ($(IGNORE_FAIL),1)
DOIT=|| true
endif

ifneq ($(CPU_TYPE),)
ifeq ($(ENDIAN),LITTLE)
QEMU_PATH ?= qemu-ppc64le-static
else ifeq ($(ENDIAN),BIG)
QEMU_PATH ?= qemu-ppc64-static
else
$(error CPU_TYPE set but ENDIAN isn't, set ENDIAN to BIG or LITTLE)
endif

ifeq ($(shell which $(QEMU_PATH) || echo fail),fail)
$(error QEMU_PATH=$(QEMU_PATH) does not appear to exist or be in $(PATH), consider manually setting QEMU_PATH or installing qemu-user-static)
endif

endif

check: $(patsubst ../bin/%,%-check,$(BINS))

sha256-%-check: ../bin/sha256-%
	BIN=$< QEMU_PATH=$(QEMU_PATH) python3 test_sha256.py -v $(DOIT)

sha512-%-check: ../bin/sha512-%
	BIN=$< QEMU_PATH=$(QEMU_PATH) python3 test_sha512.py -v $(DOIT)

