# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from __future__ import annotations

import os.path
import pathlib
import subprocess
import typing
import warnings

from typing import Any, Dict, List, Optional, Set, Type, Union

from . import ninja


if typing.TYPE_CHECKING:
    import ninja_writter


_PATH_LIST = List[Union[str, pathlib.Path]]


class DependencyError(Exception):
    pass


class Dependency():
    NAME: Optional[str] = None

    def __init__(self, *args: Any, **kwargs: Any) -> None:
        if not self.NAME:
            raise ValueError(f"Dependency doesn't have a name: {self.__class__.__name__}")

        self._base_path = pathlib.Path('external') / self.NAME
        self._dependencies: List[str] = []
        self._optional_dependencies: List[str] = []
        self._source: _PATH_LIST = []
        self._include: _PATH_LIST = []
        self._external_include: _PATH_LIST = []

    @classmethod
    def from_name(cls, name: str, *args: Any, **kwargs: Any) -> Dependency:
        return cls.class_from_name(name)(*args, **kwargs)

    @classmethod
    def class_from_name(cls, name: str) -> Type[Dependency]:
        for subclass in cls.__subclasses__():
            if subclass.NAME == name:
                return subclass
        raise ValueError(f'Could not find dependency: {name}')

    @classmethod
    def fetch_submodule(cls) -> None:
        try:
            subprocess.check_output(['git', 'submodule', 'update', '--init', f'external/{cls.NAME}'])
        except FileNotFoundError:
            warnings.warn(f'Failed to fetch dependency: {cls.NAME}')

    def write_ninja(
        self,
        nw: ninja_writter.Writter,
        dep_dic: Dict[str, Dependency],
        target_dir: str,
        config: Optional[str],
    ) -> List[str]:
        if not self.source:
            nw.comment(f'{self.NAME} has no objects')
            nw.newline()
            return []

        c_include_flags = {
            f'-I{path}' for path in self.include
        }
        c_include_flags.add(f'-I{target_dir}')

        for dep in self.dependencies:
            if dep not in dep_dic:
                raise ValueError(f'Dependency `{dep}` required by `{self.NAME}` but not specified')
            c_include_flags.update({
                f'-I{path}'
                for path in dep_dic[dep].external_include
            })

        for dep in self.optional_dependencies:
            # these dependencies will be included if found
            if dep in dep_dic:
                c_include_flags.update({
                    f'-I{nw.root(path)}'
                    for path in dep_dic[dep].external_include
                })

        if config:
            c_include_flags.add(f'-include {config}')

        nw.comment(f'{self.NAME} objects')
        nw.newline()

        objs: List[str] = []
        for file in self.source:
            objs += nw.cc_abs(file, variables=[
                ('c_include_flags', list(c_include_flags)),
            ])
        nw.newline()

        return objs

    def ninja_objects(self) -> Set[str]:
        return {ninja.Writer.built(file) for file in self._source}

    @property
    def base_path(self) -> pathlib.Path:
        return self._base_path

    @property
    def dependencies(self) -> Set[str]:
        return set(self._dependencies)

    @property
    def optional_dependencies(self) -> Set[str]:
        return set(self._optional_dependencies)

    @property
    def source(self) -> Set[str]:
        ''' Source files '''
        return {str(self.base_path / path) for path in self._source}

    @property
    def include(self) -> Set[str]:
        ''' Paths that need to be included to build the dependency source '''
        return {str(self.base_path / path) for path in self._include}

    @property
    def external_include(self) -> Set[str]:
        ''' Paths that need to be included to use the dependency '''
        return {str(self.base_path / path) for path in self._external_include}


class TinyUSBDependency(Dependency):
    NAME = 'tinyusb'

    def __init__(self, target: str) -> None:
        super().__init__()

        src_path = self.base_path / 'src'
        portable_path = src_path / 'portable'
        target_path = portable_path.joinpath(target.replace('/', os.path.sep))

        self._source = []
        for path in src_path.rglob('*.c'):
            if path.is_relative_to(target_path):
                pass
            elif path.is_relative_to(portable_path):  # not our target!
                continue
            self._source.append(
                path.relative_to(self.base_path),
            )

        self._include = [
            'src'
        ]
        self._external_include = [
            'src'
        ]
        self._optional_dependencies = [
            'cmsis-5',
            'cmsis-dfp-stm32f1',
            'cmsis-dfp-efm32gg12b',
        ]


class CMSIS5Dependency(Dependency):
    NAME = 'cmsis-5'

    def __init__(self, components: List[str] = ['Core']) -> None:
        super().__init__()
        self._components = components
        self._external_include = [
            os.path.join('CMSIS', component, 'Include')
            for component in self._components
        ]


class CMSISDeviceSTM32F1Dependency(Dependency):
    NAME = 'cmsis-dfp-stm32f1'

    def __init__(self) -> None:
        super().__init__()
        self._dependencies = ['cmsis-5']
        self._external_include = [
            'Include',
        ]


class CMSISDeviceEFM32GG12BDependency(Dependency):
    NAME = 'cmsis-dfp-efm32gg12b'

    def __init__(self) -> None:
        super().__init__()
        self._dependencies = ['cmsis-5']
        self._external_include = [
            os.path.join('Device', 'SiliconLabs', 'EFM32GG12B', 'Include'),
        ]


class SensorBlobDependency(Dependency):
    NAME = 'sensor-blobs'

    def __init__(self) -> None:
        super().__init__()
        self._external_include = [
            '.',
        ]
