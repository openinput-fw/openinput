# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import os.path
import pathlib

from typing import List

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
