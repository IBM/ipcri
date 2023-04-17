# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright 2024- IBM Corp. All rights reserved
# Authored by Eric Richter <erichte@linux.ibm.com>
#
import unittest
import os
import test_common

class TestSha512(test_common.HashTest):

    binary = os.environ.get("BIN")

    def test_nist_shortmsg(self):
        self.do_nist_test("SHA512ShortMsg.rsp")

    def test_nist_longmsg(self):
        self.do_nist_test("SHA512LongMsg.rsp")

if __name__ == "__main__":
    print(f"Testing {TestSha512.binary}")
    unittest.main()
