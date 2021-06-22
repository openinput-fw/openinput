# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>


def test_dispatch(basic_device):
    basic_device.protocol_dispatch([0x03, 0x02, 0x01])  # unrelated
    basic_device.protocol_dispatch([0x20])  # invalid length short
    basic_device.protocol_dispatch([0x21])  # invalid length long

    basic_device.hid_send.assert_not_called()


def test_fw_info_vendor(basic_device):
    basic_device.protocol_dispatch([0x20, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])

    basic_device.hid_send.assert_called_with(
        [0x21, 0x00, 0x01] + list(b'openinput-git') + [0x00] * 16
    )


def test_fw_info_version(basic_device, fw_version):
    basic_device.protocol_dispatch([0x20, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00])

    basic_device.hid_send.assert_called_with(
        [0x21, 0x00, 0x01] + list(fw_version.encode('ascii')) + [0x00] * (29 - len(fw_version))
    )


def test_fw_info_device_name(basic_device):
    basic_device.protocol_dispatch([0x20, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00])

    basic_device.hid_send.assert_called_with(
        [0x21, 0x00, 0x01] + list(b'basic test device') + [0x00] * 12
    )


def test_fw_info_unsupported(basic_device):
    basic_device.protocol_dispatch([0x20, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00])

    basic_device.hid_send.assert_called_with(
        [0x21, 0xFF, 0x01, 0x00, 0x01] + [0x00] * 27
    )
