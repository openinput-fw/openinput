# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from __future__ import annotations

import argparse
import logging
import os
import os.path
import subprocess
import typing

from typing import Any, Callable, Dict, List, Optional, Tuple, Type


abs_root = os.path.dirname(os.path.dirname(__file__))
abs_src = os.path.join(abs_root, 'src')
abs_platforms = os.path.join(abs_src, 'platform')
abs_targets = os.path.join(abs_src, 'targets')

assert abs_root
assert abs_src
assert abs_platforms
assert abs_targets


def rel_platform(name: str) -> str:
    path = os.path.join('platform', name)
    assert path
    return path


def rel_target(name: str) -> str:
    path = os.path.join('targets', name)
    assert path
    return path


def abs_platform(name: str) -> str:
    path = os.path.join(abs_platforms, name)
    assert path
    return path


def abs_target(name: str) -> str:
    path = os.path.join(abs_targets, name)
    assert path
    return path


class BuildConfigurationError(Exception):
    pass


class _BuildConfiguration():
    # default attributes
    name: Optional[str] = None
    platform: Optional[str] = None
    bin_extension: Optional[str] = None
    toolchain: Optional[str] = None
    generate_bin = True
    generate_hex = True

    # type hints
    _settings: Dict[str, List[str]]

    # internal , see _BuildConfigurationMeta
    _setting_calls: Optional[Dict[
        str,
        Callable[[_BuildConfiguration], List[str]]
    ]] = None
    _init_calls: Dict[
        Type[_BuildConfiguration],
        Callable[
            [_BuildConfiguration, Dict[str, Any]],
            None,
        ],
    ] = {}

    def __init__(
        self,
        debug: bool = False,
        toolchain: Optional[str] = None,
        linker_script: Optional[str] = None,
        **kwargs: Any,
    ) -> None:
        self.__log = logging.getLogger(self.__class__.__name__)
        self._max_len_fw_strings = 29
        self._vendor = 'openinput-git'
        self._version = 'UNKOWN'

        self.debug = debug

        # initialize settings
        self._settings = {
            'source': [
                'protocol/protocol.c',
            ],
            'include': [rel_platform('generic')],
            'include_files': [],
            'c_flags': [],
            'ld_flags': [],
        }

        # add linker script to ld_flags
        if linker_script:
            linker_scripts_dir = os.path.join(abs_root, 'build-targets', 'linker')
            self.ld_flags += [
                f'-L{linker_scripts_dir}',
                f'-T{linker_script}',
            ]

        '''
        do the magic init stuff... iterate of inheritnance chain, call init()
        for each class and append the settings
        '''
        for _cls in reversed(self.__class__.__mro__):  # __mro__ holds the inheritance chain
            cls = typing.cast(Type[_BuildConfiguration], _cls)
            self.__log.debug(f'iterating over {cls}...')
            # call init, if defined
            if cls in self._init_calls:
                self.__log.debug('calling init')
                self._init_calls[cls](self, kwargs)
            # append settings, if defined
            if hasattr(cls, '_setting_calls') and cls._setting_calls is not None:
                for setting in ('source', 'include', 'include_files', 'c_flags', 'ld_flags'):
                    if setting in cls._setting_calls:
                        self._settings[setting] += cls._setting_calls[setting](self)
                        self.__log.debug(f'appending {setting}, new value: {self._settings[setting]}')

        if toolchain is not None:
            self.toolchain = toolchain
        if self.toolchain is None:
            raise BuildConfigurationError('No toolchain specified')

    @property
    def main(self) -> str:
        if not self.name:
            raise ValueError('Not a target, there is no main')
        return os.path.join(rel_target(self.name), 'main.c')

    @staticmethod
    def parser_append_group(parser: argparse.ArgumentParser) -> None:
        pass

    # helpers

    def platform_files(self, *files: str) -> List[str]:
        if not self.platform:
            raise TypeError('Trying to get platform files from a non-platform configuration')
        return list(map(
            lambda f: os.path.join(
                rel_platform(self.platform),  # type: ignore
                *f.split('/'),
            ),
            files,
        ))

    def target_files(self, *files: str) -> List[str]:
        if not self.name:
            raise TypeError('Trying to get target files from a non-target configuration')
        return list(map(
            lambda f: os.path.join(
                rel_target(self.name),  # type: ignore
                *f.split('/'),
            ),
            files,
        ))

    def _pkgconf(self, *args: str) -> List[str]:
        try:
            return subprocess.check_output(
                ['pkgconf', '--keep-system-libs', *args],
            ).decode().strip().split()
        except FileNotFoundError:
            raise BuildConfigurationError('pkgconf not found, it is required to resolve dependencies')

    def pkgconf_libs(self, *args: str) -> List[str]:
        try:
            return self._pkgconf('--libs', *args)
        except subprocess.CalledProcessError:
            raise BuildConfigurationError(
                'Could not resolve ld_flags for the following dependencies with pkgconf: ' +
                ', '.join(args)
            )

    def pkgconf_cflags(self, *args: str) -> List[str]:
        try:
            return self._pkgconf('--cflags', *args)
        except subprocess.CalledProcessError:
            raise BuildConfigurationError(
                'Could not resolve c_flags for the following dependencies with pkgconf: ' +
                ', '.join(args)
            )

    # settings properties

    @property
    def source(self) -> List[str]:
        return self._settings['source']

    @source.setter
    def source(self, value: List[str]) -> None:
        self._settings['source'] = value

    @property
    def include(self) -> List[str]:
        return self._settings['include']

    @include.setter
    def include(self, value: List[str]) -> None:
        self._settings['include'] = value

    @property
    def include_files(self) -> List[str]:
        return self._settings['include_files']

    @include_files.setter
    def include_files(self, value: List[str]) -> None:
        self._settings['include_files'] = value

    @property
    def c_flags(self) -> List[str]:
        return self._settings['c_flags']

    @c_flags.setter
    def c_flags(self, value: List[str]) -> None:
        self._settings['c_flags'] = value

    @property
    def ld_flags(self) -> List[str]:
        return self._settings['ld_flags']

    @ld_flags.setter
    def ld_flags(self, value: List[str]) -> None:
        self._settings['ld_flags'] = value

    @property
    def vendor(self) -> str:
        return self._vendor

    @vendor.setter
    def vendor(self, value: str) -> None:
        if len(value) > self._max_len_fw_strings:
            raise ValueError()
        self._vendor = value

    @property
    def version(self) -> str:
        return self._version

    @version.setter
    def version(self, value: str) -> None:
        if len(value) > self._max_len_fw_strings:
            raise ValueError()
        self._version = value

    @property
    def vendor_c_flags(self) -> List[str]:
        return [
            fr'-DOI_VENDOR=\"{self.vendor}\"',
            fr'-DOI_VERSION=\"{self.version}\"',
        ]


class _BuildConfigurationMeta(type):
    '''
    This metaclass complements _BuildConfiguration. It has two jobs,

    1) Save the setting method (source, include, etc.) defined in the class into
    the _setting_calls dictionary. The methods will be removed from the object
    and saved into the dictionary for later use. This allows us to have each
    class define the methods they want, without overriding the parent methods
    and without having to deal with super().
    The goal here is simple, make adding new targets and build configurations as
    easy as possible. We expect people not very familiarized with Python to have
    to do it.

    2) Similarly, save the init method into the _init_calls dictionary. The
    method will be removed from the object.
    The goal of defining a init() method is to allow users to some customization
    and/or value calculations before doing anything else. This is essentially a
    boiled down, and easier to use version of __init__(), as __init__() proved
    to be very clunky and not at all beginner friendly. We just want to allow
    people to define their target and move on!

    I know metaclasses are a pain for contributors, mainly because most people
    are not comfortable with them, but the
    '''

    def __new__(mcs, name: str, bases: Tuple[Any], dic: Dict[str, Any]):  # type: ignore
        # save setting callbacks into _settings dictionary
        dic['_setting_calls'] = {}
        for setting in ('source', 'include', 'include_files', 'c_flags', 'ld_flags'):
            if setting in dic:
                dic['_setting_calls'][setting] = dic.pop(setting)

        # get init method, if defined
        init = dic.pop('init', None)

        # create the class
        cls = super().__new__(mcs, name, bases, dic)

        # append init method to _init list
        if init:
            cls._init_calls[cls] = init  # type: ignore

        return cls


class BuildConfiguration(_BuildConfiguration, metaclass=_BuildConfigurationMeta):
    pass
