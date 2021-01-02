# SPDX-License-Identifier: MIT

import os
import os.path
import platform

from typing import Any

from . import BuildConfiguration


class NativeFamily(BuildConfiguration):
    platform = 'native'
    bin_extension = 'exe' if os.name == 'nt' else ''

    def __init__(self, debug: bool = False, **kwargs: Any) -> None:
        super().__init__(debug, **kwargs)

        self.cpu_family = platform.machine()

        self.c_flags += [
            '-pipe',
            '-fno-plt',
            '-O2',
        ]

        self.ld_flags += [
            '-Wl,--sort-common,--as-needed,-z,relro,-z,now',
        ]

        if debug:
            self.c_flags += [
                '-g',
                '-fvar-tracking-assignments',
            ]


class LinuxUHIDFamily(BuildConfiguration):
    platform = 'linux-uhid'

    def __init__(self, debug: bool = False, **kwargs: Any) -> None:
        super().__init__(debug, **kwargs)

        self.source += self.platform_files(
            'hid.c',
        )

        self.include += self.platform_files(
            'hid.h',
        )


class STM32F1Family(BuildConfiguration):
    platform = 'stm32f1'
    bin_extension = 'elf'
    generate_bin = True
    generate_hex = True

    def __init__(self, debug: bool = False, **kwargs: Any) -> None:
        super().__init__(debug, **kwargs)

        self.cpu_family = 'stm32f1'

        self.c_flags += [
            '-mcpu=cortex-m3',
            '-mthumb',
        ]

        self.ld_flags += [
            '-mcpu=cortex-m3',
            '-mthumb',
            '--specs=nosys.specs',
        ]

        # self.source += self.platform_files()
