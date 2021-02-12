# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import argparse
import os
import os.path

from typing import Any, Dict, List

from . import BuildConfigurationError, abs_target, families


def _removesuffix(string: str, suffix: str) -> str:
    # str.removesuffix is only available on Python 3.9 (PEP 616)
    if suffix and string.endswith(suffix):
        return string[:-len(suffix)]
    return string


class LinuxUHIDTarget(families.NativeFamily, families.LinuxUHIDFamily):
    name = 'linux-uhid'

    def source(self) -> List[str]:
        return self.target_files(
            'main.c',
        )

    def c_flags(self) -> List[str]:
        return self.pkgconf_cflags('readline')

    def ld_flags(self) -> List[str]:
        return self.pkgconf_libs('readline') + [
            '-pthread',
        ]


class STM32F1GenericTarget(families.STM32F1Family):
    name = 'stm32f1-generic'

    def init(self, args: Dict[str, Any]) -> None:
        if 'config' not in args:
            raise BuildConfigurationError(
                f'No configuration for {self.name}, '
                'please specify one with --config'
            )

        self.config = args['config']
        if self.config not in self.configs():
            raise BuildConfigurationError(f'Unknown configuration for {self.name}: {self.config}')

    def source(self) -> List[str]:
        return self.target_files(
            'main.c',
        )

    def include_files(self) -> List[str]:
        return self.target_files(
            os.path.join('config', f'{self.config}.h'),
        )

    @classmethod
    def configs(cls) -> List[str]:
        config_path = os.path.join(abs_target(cls.name), 'config')
        return [
            config.removesuffix('.h')
            for config in os.listdir(config_path)
        ]

    @classmethod
    def parser_append_group(cls, parser: argparse.ArgumentParser) -> None:
        group = parser.add_argument_group(f'{cls.name} options')
        group.add_argument(
            '--config',
            '-c',
            type=str,
            metavar='CONFIG',
            choices=cls.configs(),
            help='device configuration name',
            required=True,
        )
        # TODO: let users specify their own files?
