# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import contextlib
import os
import os.path
import unittest.mock

import pages
import pytest
import testsuite

import build_system


root_dir = os.path.abspath(os.path.join(__file__, '..', '..'))


@contextlib.contextmanager
def cd(*path_paths):
    old_cwd = os.getcwd()
    os.chdir(os.path.join(*path_paths))
    try:
        yield
    finally:
        os.chdir(old_cwd)


@pytest.fixture()
def device(request):
    return testsuite.Device(
        name='test device',
        functions=request.param,
    )


@pytest.fixture()
def basic_device():
    device = testsuite.Device(
        name='basic test device',
        functions={
            pages.Info.VERSION,
            pages.Info.FW_INFO,
            pages.Info.SUPPORTED_FUNCTION_PAGES,
            pages.Info.SUPPORTED_FUNCTIONS,
        },
    )
    device.hid_send = unittest.mock.MagicMock()
    return device


@pytest.fixture()
def fw_version():
    return build_system.VersionInfo.from_git().full_string
