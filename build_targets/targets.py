# SPDX-License-Identifier: MIT

import argparse
import os
import os.path

from typing import Any, Optional

from . import BuildConfigurationError, abs_target, families


def _removesuffix(string: str, suffix: str) -> str:
    # str.removesuffix is only available on Python 3.9 (PEP 616)
    if suffix and string.endswith(suffix):
        return string[:-len(suffix)]
    return string


class LinuxUHIDTarget(families.NativeFamily, families.LinuxUHIDFamily):
    name = 'linux-uhid'

    def __init__(self, debug: bool = False, **kwargs: Any) -> None:
        super().__init__(
            debug,
            cross_toolchain=None,
            **kwargs,
        )

        self.source += self.target_files(
            'main.c',
        )


class STM31F1GenericTarget(families.STM32F1Family):
    name = 'stm32f1-generic'
    configs = list(map(
        lambda f: _removesuffix(f, '.h'),
        os.listdir(os.path.join(abs_target(name), 'config'))
    ))

    def __init__(
        self,
        debug: bool = False,
        *,
        config: Optional[str] = None,
        **kwargs: Any,
    ) -> None:
        super().__init__(
            debug,
            cross_toolchain='arm-none-eabi',
            **kwargs,
        )

        if not config:
            raise BuildConfigurationError(
                f'No configuration for {self.name}, '
                'please specify one with --config'
            )
        if config not in self.configs:
            raise BuildConfigurationError(f'Unknown configuration for {self.name}: {config}')

        self.source += self.target_files(
            'main.c',
        )

        self.include += self.platform_files(
            os.path.join('config', f'{config}.h'),
        )

    @classmethod
    def parser_append_group(cls, parser: argparse.ArgumentParser) -> None:
        group = parser.add_argument_group(f'{cls.name} options')
        group.add_argument(
            '--config',
            '-c',
            type=str,
            metavar='CONFIG',
            choices=cls.configs,
            help='device configuration name',
            required=True,
        )
        # TODO: let users specify their own files?
