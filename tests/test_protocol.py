# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import pages
import pytest
import testsuite


@pytest.fixture()
def device():
    return testsuite.Device(
        name='test device',
        functions={
            pages.Info.VERSION,
            pages.Info.FW_INFO,
            pages.Info.SUPPORTED_FUNCTION_PAGES,
            pages.Info.SUPPORTED_FUNCTIONS,
        },
    )


def test_something(device):
    device.protocol_dispatch([0x10])
