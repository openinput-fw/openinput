# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import ctypes
import os.path

import pytest


testsuite_out = os.path.abspath(os.path.join(
    __file__, '..', '..', 'build', 'testsuite', 'out',
))


@pytest.fixture()
def testlib():
    return ctypes.cdll.LoadLibrary(os.path.join(testsuite_out, 'testsuite.so'))
