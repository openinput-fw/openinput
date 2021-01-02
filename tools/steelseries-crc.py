#!/usr/bin/env python3

import argparse
import os
import sys
import struct

if __name__ == '__main__':  # noqa: C901
    parser = argparse.ArgumentParser(description='SteelSeries crc calculation')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Shows debugging outputs.')
    parser.add_argument('-a', '--append',
                        action='store_true',
                        help='Appends the crc result to the file'
                             '(by default outputs only the crc, this prepends the firmware to the output).')
    parser.add_argument('-o', '--output', type=str,
                        help='Output file.')
    parser.add_argument('firmware', metavar='firmware.bin', type=str,
                        help='Firmware to calculate crc of.')
    args = parser.parse_args()

    if not args.firmware or not os.path.exists(args.firmware):
        print(f'{args.firmware} doesn\'t exist', file=sys.stderr)
        exit(1)

    if args.firmware == args.output:
        print('output file can\'t be the same as the input file', file=sys.stderr)
        exit(1)

    try:
        firmware = open(args.firmware, 'rb')
    except PermissionError as e:
        print(f'can\'t open {args.firmware}: {str(e)}', file=sys.stderr)
        exit(1)

    if args.output:
        try:
            output = open(args.output, 'wb')
        except PermissionError as e:
            print(f'can\'t open {args.output}: {str(e)}', file=sys.stderr)
            exit(1)

    num_bytes = os.path.getsize(args.firmware)
    remainder_bytes = num_bytes % 4

    if args.verbose:
        print(f'Firmware length: {num_bytes}')
        print(f'Unaligned bytes: {remainder_bytes}')

    num_words = int((num_bytes - remainder_bytes) / 4)

    firmware_bytes = bytearray(firmware.read(num_bytes))

    if remainder_bytes:
        num_words += 1
        firmware_bytes.extend([0] * (4 - remainder_bytes))

    firmware_words = struct.unpack('I'*num_words, firmware_bytes)

    crc = 0xFFFFFFFF

    for word in firmware_words:
        crc ^= word
        for i in range(32):
            if crc & 0x80000000:
                crc = (crc << 1) ^ 0x04C11DB7
            else:
                crc = (crc << 1)
        crc &= 0xFFFFFFFF

    crc_bytes = struct.pack("<I", crc)

    if args.verbose:
        print(f'calculated crc: 0x{crc:08x}')

    if not args.output or args.verbose:
        if args.append:
            print(bytes(firmware_bytes).hex() + bytes(crc_bytes).hex())
        else:
            print(bytes(crc_bytes).hex())

    if args.output:
        if args.append:
            output.write(firmware_bytes)
        output.write(crc_bytes)
