#!/usr/bin/env python3

import argparse
import os
import sys

from typing import List

import hidraw


def ljust(buf: List[int], size: int, char: int = 0x00) -> List[int]:
    return buf + [char] * (size - len(buf))


def flatten(items: List) -> List[int]:
    '''
    Flattens lists

    Example:
        flatten([0x01, 0x02, [0x03, 0x04, 0x05], 0x06]) == [0x01, 0x02, 0x03, 0x04, 0x05, 0x06]
    '''
    ret = []
    for item in items:
        if isinstance(item, list):
            for i in item:
                ret.append(i)
        else:
            ret.append(item)
    return ret


def to_le(size: int, val: int) -> List[int]:
    assert size % 8 == 0
    buf = []
    for i in range(0, size, 8):
        buf.append((val & (0xff << i)) >> i)
    return buf


def str_buf(buf: List[int]) -> str:
    return ''.join(f'{byte:02x}' for byte in buf)


class SteelseriesProtocol:
    @classmethod
    def start(cls) -> List[int]:
        return ljust([0x70, 0x00, 0x01], 64)

    @classmethod
    def write(cls, address: int, data: List[int]) -> List[int]:
        size = len(data)
        assert size <= 48
        return ljust(flatten([0x03, 0x00, address, 0x00, size, 0x00, data[:size]]), 64)

    @classmethod
    def commit(cls, sector: int, address: int, size: int) -> List[int]:
        return ljust(flatten([0x05, 0x00, sector, 0x00, to_le(16, address), size]), 64)

    @classmethod
    def reset(cls) -> List[int]:
        return ljust([0x01], 64)


if __name__ == '__main__':  # noqa: C901
    parser = argparse.ArgumentParser(description='SteelSeries fimware update')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Shows raw packets.')
    parser.add_argument('-s', '--sensor',
                        action='store_true',
                        help='Update the sensor firmware instead of the main firmware.')
    parser.add_argument('device', metavar='/dev/hidrawX', type=str,
                        help='Hidraw device on which to perform the update.')
    parser.add_argument('firmware', metavar='firmware.bin', type=str,
                        help='Firmware to write to the device.')
    args = parser.parse_args()

    if not args.device or not os.path.exists(args.device):
        print(f'{args.device} doesn\'t exist', file=sys.stderr)
        exit(1)

    try:
        device = hidraw.Hidraw(args.device)
    except PermissionError as e:
        print(f'can\'t open {args.device}: {str(e)}', file=sys.stderr)
        exit(1)

    if not args.firmware or not os.path.exists(args.firmware):
        print(f'{args.firmware} doesn\'t exist', file=sys.stderr)
        exit(1)

    try:
        firmware = open(args.firmware, 'rb')
    except PermissionError as e:
        print(f'can\'t open {args.firmware}: {str(e)}', file=sys.stderr)
        exit(1)

    sector = 2 if not args.sensor else 3

    print('Starting update...')
    device.command_raw(SteelseriesProtocol.start())

    address = 0x0000
    while True:
        buf = list(firmware.read(0x80))

        if not buf:
            break

        written = 0
        for i in range(0, 0x80, 0x30):
            cmd_buf = buf[i:i + 0x30]
            device.command_raw(SteelseriesProtocol.write(address=i, data=cmd_buf))
            written += len(cmd_buf)
            if len(cmd_buf) < 0x30:
                break

        device.command_raw(SteelseriesProtocol.commit(sector, address, written))

        address += 0x80

    device.command_raw(SteelseriesProtocol.reset())
