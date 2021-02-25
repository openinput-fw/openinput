# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import os
import os.path

from typing import Any, Dict, List

from . import BuildConfiguration, BuildDependency
from .dependencies import CMSISDependency, CMSISDeviceSTM32F1Dependency, TinyUSBDependency


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

    def source(self) -> List[str]:
        return self.platform_files(
            'startup.c',
            'exceptions.c',
            'flash.c',
            'rcc.c',
            'systick.c',
            'gpio.c',
            'hal/spi.c',
            'spi.c',
        )

    def c_flags(self) -> List[str]:
        return [
            '-Os',
            '-nostdlib',
            '-nostartfiles',
            '-ffunction-sections',
            '-fdata-sections',
            '-march=armv7-m',
            '-mtune=cortex-m3',
            '-mthumb',
            '-mfloat-abi=soft',
            '-ffreestanding',
        ]

    def ld_flags(self) -> List[str]:
        return [
            '-lm',
            '-lc',
            '-lgcc',
            '-lnosys',
            '-nostdlib',
            '-nostartfiles',
            '-fdata-sections',
            '-ffunction-sections',
            '--specs=nano.specs',
            '--specs=nosys.specs',
            '-Wl,--gc-sections',
            '-march=armv7-m',
            '-mtune=cortex-m3',
            '-mthumb',
        ]

    def dependencies(self) -> List[BuildDependency]:
        cmsis_deps = [
            CMSISDependency(components=['Core']),
            CMSISDeviceSTM32F1Dependency(),
        ]
        return cmsis_deps + [
            TinyUSBDependency(dependencies=cmsis_deps),
        ]
