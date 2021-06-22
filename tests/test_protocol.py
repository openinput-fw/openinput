# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>


def test_dispatch(basic_device):
    basic_device.protocol_dispatch([0x03, 0x02, 0x01])  # unrelated
    basic_device.protocol_dispatch([0x20])  # invalid length short
    basic_device.protocol_dispatch([0x21])  # invalid length long

    basic_device.hid_send.assert_not_called()
