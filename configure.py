#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from __future__ import annotations

import argparse
import pathlib
import sys
import traceback
import warnings

from typing import List, Optional, TextIO, Type, Union

import build_system
import build_system.builder


_MIN_PYTHON_VERSION = (3, 9)


# disable colors if we're not in a TTY
if sys.stdout.isatty():
    _RESET = '\33[0m'
    _DIM = '\33[2m'
    _RED = '\33[91m'
    _CYAN = '\33[96m'
else:
    _RESET = ''
    _DIM = ''
    _RED = ''
    _CYAN = ''


def _error(msg: str, code: int = 1) -> None:
    '''
    Prints an error message and exit with code
    '''
    print(f'{_RED}ERROR{_RESET} {msg}')
    exit(code)


if sys.version_info < _MIN_PYTHON_VERSION:
    _error(
        f'Unsupported Python version: {".".join(map(str, sys.version_info[:3]))}... '
        f'You need at least Python {".".join(map(str, _MIN_PYTHON_VERSION))}.'
    )


# external imports with nice error messages for better UX

try:
    import tomli  # noqa: F401
except ImportError:
    _error(
        'Missing tomli dependency!\n'
        'You can install it with: pip install tomli'
    )

try:
    import ninja_syntax  # noqa: F401
except ImportError:
    _error(
        'Missing ninja_syntax dependency!\n'
        'You can install it with: pip install ninja_syntax'
    )
else:
    import build_system.dependencies
    import build_system.ninja


# overide default warning handler for a nice CLI

def _showwarning(
    message: Union[Warning, str],
    category: Type[Warning],
    filename: str,
    lineno: int,
    file: Optional[TextIO] = None,
    line: Optional[str] = None,
) -> None:
    '''
    Show warning to the console

    We override warnings.showwarning with this to match our CLI
    '''
    prefix = 'WARNING'
    if sys.stdout.isatty():
        prefix = '\33[93m' + prefix + '\33[0m'
    print('{} {}'.format(prefix, str(message)))


warnings.showwarning = _showwarning


def _ask_target(targets: List[str]) -> str:
    print('Available targets:')
    for i, available_target in enumerate(targets):
        print(f'{available_target:>32} ({i})')
    while True:
        select = input('Select target (default=linux-uhid): ')
        if not select:
            target = 'linux-uhid'
            break
        try:
            target = targets[int(select)]
            break
        except (ValueError, IndexError):
            print('Invalid option!')
    print()
    return target


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--builddir',
        '-b',
        type=str,
        default='build',
        help='build directory',
    )
    parser.add_argument(
        '--toolchain',
        '-t',
        type=str,
        help='override toolchain prefix (eg. arm-none-eabi)',
    )
    parser.add_argument(
        '--compiler',
        '-p',
        default='gcc',
        choices=('gcc', 'clang'),
        help='compiler to use',
    )
    parser.add_argument(
        '--debug',
        '-d',
        action='store_true',
        help='output extra debug information',
    )
    parser.add_argument(
        '--no-fetch',
        '-n',
        action='store_true',
        help="don't fetch dependency git submodules",
    )
    target_subparsers = parser.add_subparsers(
        dest='target',
        help='target firmware',
        metavar='TARGET',
    )
    # XXX: Add key to target_args to be ignored after adding new arguments!

    location = build_system.BuildLocation(
        pathlib.Path(__file__).absolute().parent,
        pathlib.Path('dummy'),
    )
    config_loader = build_system.ConfigLoader(location)

    # register target config arguments
    for target in config_loader.available_targets:
        target_parser = target_subparsers.add_parser(target)
        if target in config_loader.available_target_configs:
            target_parser.add_argument(
                '--config',
                '-c',
                type=str,
                metavar='CONFIG',
                choices=config_loader.available_target_configs[target],
                help='device configuration name',
                required=True,
            )

    args = parser.parse_args()
    # XXX: This isn't great but we don't have a better way AFAIK.
    target_args = {
        key: value
        for key, value in vars(args).items()
        if key not in (
            'builddir',
            'toolchain',
            'compiler',
            'debug',
            'no_fetch',
            'target',
            'config',
        )
    }

    if not args.target:
        # ask for target if it was not specified
        args = parser.parse_args([_ask_target(
            list(config_loader.available_targets)
        )])

    try:
        builder = build_system.builder.Builder(
            build_system.BuildLocation(location.source, args.builddir),
            build_system.TargetInfo(
                name=args.target,
                config=args.config if 'config' in args else None,
                type='firmware',
                args=target_args,
            ),
            build_system.BuildSettings(
                mode='debug' if args.debug else 'release',
                compiler=args.compiler,
            ),
            build_system.VendorInfo(),
            build_system.VersionInfo.from_git(),
        )
        builder.print_summary()
        builder.write_ninja()
        print("\nbuild.ninja written! Call 'ninja' to compile...")
    except Exception as e:
        print()
        print(_DIM + traceback.format_exc() + _RESET)
        _error(str(e))
