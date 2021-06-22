# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import contextlib
import importlib.util
import os
import os.path
import unittest.mock

import pages
import pytest
import testsuite


root_dir = os.path.abspath(os.path.join(__file__, '..', '..'))


@contextlib.contextmanager
def cd(*path_paths):
    old_cwd = os.getcwd()
    os.chdir(os.path.join(*path_paths))
    try:
        yield
    finally:
        os.chdir(old_cwd)


def import_file(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    if not spec.loader:
        raise ImportError(f'Unable to import `{path}`: no loader')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)  # type: ignore
    return module


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
    with cd(root_dir):
        configure = import_file('configure', 'configure.py')
        version = configure.BuildSystemBuilder.calculate_version()
        is_dirty = configure.BuildSystemBuilder._is_git_dirty()
    return f'{version}.dirty' if is_dirty else version
