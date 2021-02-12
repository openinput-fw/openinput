# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import os
import os.path

from typing import Any, Dict, List

from . import BuildConfiguration


class NativeFamily(BuildConfiguration):
    def init(self, args: Dict[str, Any]) -> None:
        self.platform = 'native'
        self.toolchain = ''  # native
        self.bin_extension = 'exe' if os.name == 'nt' else ''
        self.generate_bin = False
        self.generate_hex = False

    def c_flags(self) -> List[str]:
        flags = [
            '-pipe',
            '-fno-plt',
            '-O2',
            '-fsanitize=address',
            '-g',
        ]

        if self.debug:
            flags += [
                '-g',
                '-fvar-tracking-assignments',
            ]

        return flags

    def ld_flags(self) -> List[str]:
        return [
            '-Wl,--sort-common,--as-needed,-z,relro,-z,now',
            '-fsanitize=address',
            '-g',
        ]


class LinuxUHIDFamily(BuildConfiguration):
    def init(self, args: Dict[str, Any]) -> None:
        self.platform = 'linux-uhid'

    def source(self) -> List[str]:
        return self.platform_files(
            'hal/hid.c',
            'uhid.c',
        )


class STM32F1Family(BuildConfiguration):
    platform = 'stm32f1'

    def init(self, args: Dict[str, Any]) -> None:
        self.toolchain = 'arm-none-eabi'
        self.bin_extension = 'elf'
        self.generate_bin = True
        self.generate_hex = True

    def c_flags(self) -> List[str]:
        return [
            '-Os',
            '-march=armv7-m',
            '-mtune=cortex-m3',
            '-mthumb',
        ]

    def ld_flags(self) -> List[str]:
        return [
            '-march=armv7-m',
            '-mtune=cortex-m3',
            '-mthumb',
            '--specs=nosys.specs',
        ]
