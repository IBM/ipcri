# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024- IBM Corp. All rights reserved
# Authored by Eric Richter <erichte@linux.ibm.com>
#
from subprocess import check_output as px
import unittest
import re
import os
import sys

length_re = re.compile(r"Len = (\d+)")
msg_re = re.compile(r"Msg = ([a-f0-9]+)")
digest_re = re.compile(r"MD = ([a-f0-9]+)")

class Case:
    length: int
    msg: str
    digest: str

    def __init__(self, length, msg, digest):
        self.length = length
        self.msg = msg
        self.digest = digest

    @staticmethod
    def from_lines(lines):
        length = int(length_re.match(lines[0]).group(1))
        msg = msg_re.match(lines[1]).group(1)
        digest = digest_re.match(lines[2]).group(1)

        return Case(length, msg, digest)
    
def load_rspfile(name):
    # Assume data is relative to the directory this test case lives in
    path = f"./cases/{name}"

    with open(path, "r") as f:
        # Filter out the empty lines, comment lines, and the one line that says how many there are
        lines = list(filter(lambda x: x and not x.startswith("#") and not x.startswith("["), map(str.strip, f)))

    # Split into chunks of three
    lines = [lines[i:i+3] for i in range(0, len(lines), 3)]
    # Parse into a list of Cases
    data = list(map(Case.from_lines, lines))

    return data

def run_hash(binpath, case, args=[], qemu_path=[], cpu_type=[]):
    # TODO: either do this without a tmpfile, or figure a more graceful way to handle this
    tmpfile = "/tmp/ipcri_testdata"

    with open(tmpfile, "wb") as f:
        # Open an empty file for the empty case
        if case.length != 0:
            f.write(bytes.fromhex(case.msg))

    command = list(filter(lambda x: x, qemu_path + cpu_type + [binpath] + args + [tmpfile]))
    output = px(command, universal_newlines=True)
    output = output.strip()
    return output

class HashTest(unittest.TestCase):

    # To be set by the implementing test case
    binary = None
    cpu_type = []
    qemu_path = []

    def setUp(self):
        self.cpu_type = os.environ.get("CPU_TYPE", [])
        if self.cpu_type:
            self.assertTrue(self.cpu_type in ["power8", "power9", "power10"])
            self.cpu_type = ["-cpu", self.cpu_type.lower()]

        self.qemu_path = [os.environ.get("QEMU_PATH")]

        if self.cpu_type and not self.qemu_path:
            print(self.cpu_type, self.qemu_path)
            print("CPU_TYPE set, but QEMU_PATH not set, probably a Makefile bug?")
            sys.exit(1)

    def do_nist_test(self, filename, args=[]):
        self.assertTrue(not not self.binary)

        cases = load_rspfile(filename)

        self.assertTrue(cases)

        for c in cases:
            self.assertEqual(run_hash(self.binary, c, args=args, qemu_path=self.qemu_path, cpu_type=self.cpu_type), c.digest)

