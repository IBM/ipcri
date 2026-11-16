#!/bin/bash
#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright IBM Corp. 2026
# Authored by Eric Richter <erichte@linux.ibm.com>
#

SUPPORT=0

# Default to gcc if not set
if [ -z "$CC" ]; then
  # Try to use CROSS if that happens to exist
  CC="${CROSS}gcc"
fi

# Make sure we have a POWER compiler
if ! $CC -dumpmachine | grep -E "p(ower)?pc64" > /dev/null ; then
  echo "Compiler '$CC' does not appear to compile for POWER." >&2
  echo "Please set either \$CC or \$CROSS." >&2
  exit 2
fi

test_instr() {
  if [ -z "$DEBUG" ]; then
    echo "$1" | $CC -c -x assembler -o /dev/null - > /dev/null 2>&1
  else
    echo "$1" | $CC -c -x assembler -o /dev/null -
  fi
}
# $1 - rfc testing function to call. should return 0 if rfc appears supported, 1 otherwise
# $2 - bit index for the rfc. e.g. 2 means bit 2 should be set if the rfc is supported
test_rfc() {
  echo -n "$1 " >&2
  if $1 ; then
    echo "Supported" >&2
    SUPPORT=$((SUPPORT|1<<$2))
  else
    echo "Not Supported" >&2
  fi
}


### RFC Check functions

rfc_02653() {
  test_instr "dmsetdmrz 1"            && \
  test_instr "dmxxinstdmr256 0,2,0"   && \
  test_instr "dmxxinstdmr512 0,2,4,0" && \
  test_instr "dmxxextfdmr256 0,2,0"   && \
  test_instr "dmxxextfdmr512 0,2,4,0"
}

rfc_02654() {
  test_instr "dmsha2hash 0,1,1"    && \
  test_instr "dmsha3hash 0"
}

rfc_02657() {
  test_instr "xxaesencp 0,2,4,0"
}

test_rfc rfc_02653 0
test_rfc rfc_02654 1
test_rfc rfc_02657 2

printf "0x%x\n" $SUPPORT
