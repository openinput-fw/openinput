# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from __future__ import annotations

import dataclasses
import os.path
import pathlib
import subprocess
import warnings

from typing import Any, ClassVar, Dict, List, Optional, Set, Type

from . import BuildLocation
from .ninja import NinjaBuilder


class DependencyError(Exception):
    pass


@dataclasses.dataclass(init=False)
class Dependency():
    name: ClassVar[str]
    location: BuildLocation
    dependencies: Set[str]
    optional_dependencies: Set[str]
    source: Set[pathlib.Path]
    include: Set[pathlib.Path]
    external_include: Set[pathlib.Path]

    def __init_subclass__(cls, name: str) -> None:
        cls.name = name

    def __init__(self, location: BuildLocation) -> None:
        self.location = location
        self.dependencies = set()
        self.optional_dependencies = set()
        self.source = set()
        self.include = set()
        self.external_include = set()

    @classmethod
    def from_name(cls, name: str, *args: Any, **kwargs: Any) -> Dependency:
        return cls.class_from_name(name)(*args, **kwargs)

    @classmethod
    def class_from_name(cls, name: str) -> Type[Dependency]:
        for subclass in cls.__subclasses__():
            if subclass.name == name:
                return subclass
        raise ValueError(f'Could not find dependency: {name}')

    @classmethod
    def fetch_submodule(cls) -> None:
        try:
            subprocess.check_output(['git', 'submodule', 'update', '--init', f'external/{cls.name}'])
        except FileNotFoundError:
            warnings.warn(f'Failed to fetch dependency: {cls.name}')

    @property
    def base_path(self) -> pathlib.Path:
        return self.location.source / 'external' / self.name

    def write_ninja(
        self,
        nb: NinjaBuilder,
        dependency_dictionary: Dict[str, Dependency],
        target_dir: pathlib.Path,
        config: Optional[pathlib.Path],
    ) -> List[str]:
        if not self.source:
            nb.writer.comment(f'{self.name} has no objects')
            nb.writer.newline()
            return []

        c_include_flags = {
            f'-I{nb.path(path)}' for path in self.include
        }
        c_include_flags.add(f'-I{nb.path(target_dir)}')

        for dep in self.dependencies:
            if dep not in dependency_dictionary:
                raise ValueError(f'Dependency `{dep}` required by `{self.name}` but not specified')
            c_include_flags.update({
                f'-I{nb.path(path)}'
                for path in dependency_dictionary[dep].external_include
            })

        for dep in self.optional_dependencies:
            # these dependencies will be included if found
            if dep in dependency_dictionary:
                c_include_flags.update({
                    f'-I{nb.path(path)}'
                    for path in dependency_dictionary[dep].external_include
                })

        if config:
            c_include_flags.add(f'-include {nb.path(config)}')

        nb.writer.comment(f'{self.name} objects')
        nb.writer.newline()

        objs: List[str] = []
        for file in self.source:
            objs += nb.cc(file, variables=[
                ('c_include_flags', list(c_include_flags)),
            ])
        nb.writer.newline()

        return objs


@dataclasses.dataclass(init=False)
class TinyUSBDependency(Dependency, name='tinyusb'):
    target: str

    def __init__(self, location: BuildLocation, target: str) -> None:
        super().__init__(location)

        src_path = self.base_path / 'src'
        portable_path = src_path / 'portable'
        target_path = portable_path / target.replace('/', os.path.sep)

        self.target = target

        for path in src_path.rglob('*.c'):
            if path.is_relative_to(target_path):
                pass
            elif path.is_relative_to(portable_path):  # not our target!
                continue
            self.source.add(path)

        self.include = {
            src_path,
            self.location.code,
        }
        self.external_include = {
            src_path,
        }
        self.optional_dependencies = {
            'cmsis-5',
            'cmsis-dfp-stm32f1',
            'cmsis-dfp-efm32gg12b',
        }


@dataclasses.dataclass(init=False)
class CMSIS5Dependency(Dependency, name='cmsis-5'):
    components: List[str]

    def __init__(self, location: BuildLocation, components: List[str] = ['Core']) -> None:
        super().__init__(location)
        self.components = components
        self.external_include = {
            self.base_path / 'CMSIS' / component / 'Include'
            for component in self.components
        }


class CMSISDeviceSTM32F1Dependency(Dependency, name='cmsis-dfp-stm32f1'):
    def __init__(self, location: BuildLocation) -> None:
        super().__init__(location)
        self.dependencies = {
            'cmsis-5',
        }
        self.external_include = {
            self.base_path / 'Include',
        }


class CMSISDeviceEFM32GG12BDependency(Dependency, name='cmsis-dfp-efm32gg12b'):
    def __init__(self, location: BuildLocation) -> None:
        super().__init__(location)
        self.dependencies = {
            'cmsis-5',
        }
        self.external_include = {
            self.base_path / 'Device' / 'SiliconLabs' / 'EFM32GG12B' / 'Include',
        }


class SensorBlobDependency(Dependency, name='sensor-blobs'):
    def __init__(self, location: BuildLocation) -> None:
        super().__init__(location)
        self.external_include = {
            self.base_path,
        }
