#!/usr/bin/env python

import argparse
import sys
import os

import hidraw

devices = {}


def get_devices():
    for fname in os.listdir('/dev/'):
        if fname.startswith('hidraw'):
            yield fname


def add_device(path):
    if os.path.exists(path):
        try:
            devices[path] = hidraw.Hidraw(path)
        except IOError as e:
            print(f"can't open '{path}': {str(e)}", file=sys.stderr)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='List HID devices')
    parser.add_argument('-s', metavar='devnum',
                        type=int, dest='devnum',
                        help='Show only devices in specified devnum. ID is given in decimal.')
    parser.add_argument('-D', metavar='/dev/hidrawX',
                        type=str, dest='file',
                        help='Do not scan the /dev/ directory, instead display only information about the device whose device file is given.')
    args = parser.parse_args()

    if args.devnum is not None:
        add_device(f'/dev/hidraw{args.devnum}')
    elif args.file is not None:
        add_device(args.file)
    else:
        for device in get_devices():
            add_device(f'/dev/{device}')

    if not devices:
        exit(1)

    for key, device in devices.items():
        print(f'Device {key} Bus {device.info.bus:03d}: ID {device.info.vid:04x}:{device.info.pid:04x} {device.name}'
              f'{" (supports vendor packets)" if device.has_vendor_page else ""}')
