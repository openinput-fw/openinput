# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import os.path
import pathlib

from typing import Dict, List

from . import BuildDependency


def rel_external(*subdirs: str) -> str:
    path = os.path.join('external', *subdirs)
    assert path
    return path


class TinyUSBDependency(BuildDependency):
    name = 'tinyusb'

    def source(self) -> List[str]:
        return [
            str(path)
            for path in pathlib.Path(rel_external('tinyusb', 'src')).rglob('*.c')
        ]

    def include(self) -> List[str]:
        return [
            rel_external('tinyusb', 'src'),
        ]

    def external_include(self) -> List[str]:
        return [
            rel_external('tinyusb', 'src'),
        ]


class CMSISDependency(BuildDependency):
    name = 'cmsis-5'

    def init(self, settings: Dict[str, str]) -> None:
        self._components = settings['components']

    def external_include(self) -> List[str]:
        return [
            rel_external(self.name, 'CMSIS', component, 'Include')
            for component in self._components
        ]


class CMSISDeviceSTM32F1Dependency(BuildDependency):
    name = 'cmsis-dfp-stm32f1'

    def external_include(self) -> List[str]:
        return [
            rel_external(self.name, 'Include'),
        ]


class SensorBlobDependency(BuildDependency):
    name = 'sensor-blobs'

    def external_include(self) -> List[str]:
        return [
            rel_external(self.name),
        ]
