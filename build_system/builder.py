# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from __future__ import annotations

import os
import pathlib
import shutil
import sys
import warnings

from typing import Dict, List, Optional

from . import BuildDetails, BuildLocation, BuildSettings, ConfigLoader, TargetInfo, VendorInfo, VersionInfo
from .dependencies import Dependency
from .ninja import NinjaBuilder


if sys.stdout.isatty():
    _RESET = '\33[0m'
    _DIM = '\33[2m'
    _RED = '\33[91m'
    _CYAN = '\33[96m'
else:
    _RESET = ''
    _DIM = ''
    _RED = ''
    _CYAN = ''


class ConfigurationError(Exception):
    """Exception when configuring the build system."""


class Builder:
    """Project builder."""

    _REQUIRED_TOOLS = (
        'ar',
        'gcc',
        'objcopy',
        'size',
    )

    def __init__(
        self,
        location: BuildLocation,
        target: TargetInfo,
        settings: BuildSettings,
        vendor: VendorInfo,
        version: VersionInfo,
        fetch: bool = True,
    ) -> None:
        self._location = location
        self._target = target
        self._settings = settings
        self._vendor = vendor
        self._version = version
        self._fetch = fetch

        self._config = ConfigLoader(location).get_config(target)
        self._details = BuildDetails(target, self._config, settings, location, vendor, version)

        self._validate_toolchain()
        self._calculate_extensions()
        self._populate_dependencies()

    def _validate_toolchain(self) -> None:
        if self._settings.toolchain is not None:  # override toolchain
            warnings.warn('Overriding target toolchain, may not be compatible!')
        else:  # use the toolchain defined by the target
            self._settings.toolchain = self._config.target.get(
                'toolchain',
                self._config.family.get('toolchain'),
            )

        if self._settings.toolchain:
            if self._config.target.get('is-shared-library'):
                raise ConfigurationError('`is-shared-library` not available when cross compiling')
            if self._settings.toolchain.endswith('-'):
                warnings.warn(
                    f'Specified `{self._settings.toolchain}` as the cross toolchain, '
                    f'did you mean `{self._settings.toolchain[:-1]}`?'
                )
        for tool in self._REQUIRED_TOOLS:
            name = '-'.join(filter(None, [self._settings.toolchain, tool]))
            if shutil.which(name) is None:
                raise ConfigurationError(f'Invalid toolchain: {name} not found')

    def _calculate_extensions(self) -> None:
        if not self._settings.toolchain:  # native
            if self._config.target.get('is-shared-library'):
                self._bin_extension = 'dll' if os.name == 'nt' else 'so'
            else:
                self._bin_extension = 'exe' if os.name == 'nt' else ''
            self._generate_bin = False
            self._generate_hex = False
        else:
            self._bin_extension = 'elf'
            self._generate_bin = True
            self._generate_hex = True

    def _populate_dependencies(self) -> None:
        self._dependencies: Dict[str, Dependency] = {}

        dependencies = self._config.family.get('dependencies', {})
        dependencies.update(
            self._config.target.get('dependencies', {})
        )

        try:
            for name, kwargs in dependencies.items():
                cls = Dependency.class_from_name(name)
                if self._fetch:
                    cls.fetch_submodule()
                self._dependencies[name] = cls(self._location, **kwargs)  # type: ignore[call-arg]
        except TypeError as e:
            raise ConfigurationError(f'Failed to initialize dependency `{name}`: {str(e)}')

    @property
    def _out_name(self) -> str:
        if 'out-name' in self._config.target:
            name = self._config.target['out-name']
            assert isinstance(name, str)
            return name
        return '-'.join(filter(None, [
            'openinput',
            self._target.name,
            self._version.string.replace('.', '-'),
            'dirty' if self._version.dirty else None,
        ]))

    def _out_path(self, extension: str) -> pathlib.Path:
        file_name = '.'.join(filter(None, [self._out_name, extension]))
        return pathlib.Path('out', file_name)

    def write_ninja(self, file: str = 'build.ninja') -> None:
        with NinjaBuilder.from_path(file, self._location, self._settings, self._target) as nb:
            nb.write_header(sys.argv[0])
            nb.write_variables(self._details, self._dependencies)
            nb.write_rules()

            objs: List[str] = []

            # optional config argument
            config: Optional[pathlib.Path] = None
            if self._target.config:
                config = self._location.code / 'targets' / self._target.name / 'config' / f'{self._target.config}.h'

            # build dependencies
            for name, dep in self._dependencies.items():
                target_path = self._location.code / 'targets' / self._target.name
                objs += dep.write_ninja(nb, self._dependencies, target_path, config)

            # build our source
            nb.writer.comment('target objects')
            nb.writer.newline()
            for src_file in self._details.source:
                objs += nb.cc(src_file)
            nb.writer.newline()

            # build output objects
            nb.writer.comment('target')
            nb.writer.newline()
            bin_out = self._out_path(self._bin_extension)
            if self._config.target.get('is-shared-library') is True:
                bin_obj = nb.build(bin_out, 'link', objs, variables={
                    'ld_flags': self._details.ld_flags + ['-shared']
                })
            else:
                bin_obj = nb.build(bin_out, 'link', objs)
            nb.writer.newline()

            if self._generate_bin:
                nb.build(self._out_path('bin'), 'bin', bin_obj)
                nb.writer.newline()
            if self._generate_hex:
                nb.build(self._out_path('hex'), 'hex', bin_obj)
                nb.writer.newline()

    def print_summary(self) -> None:
        print(
            f'openinput {_CYAN}{self._version.string}{_RESET}' +
            (f' {_RED}(dirty){_RESET}' if self._version.dirty else ''),
            end='\n\n'
        )

        for name, var in {
            'target': self._target.name,
            'toolchain': self._settings.toolchain or 'native',
            'c_flags': ' '.join(self._details.c_flags) or '(empty)',
            'ld_flags': ' '.join(self._details.ld_flags) or '(empty)',
            'dependencies': ' '.join([dep for dep in self._dependencies]) or None
        }.items():
            if var:
                print('{:>24}: {}'.format(name, var))

        for name, entries in {
            'source': self._details.source,
            'include_files': self._details.include_files,
        }.items():
            print('{:>24}:'.format(name))
            if not entries:
                print(' ' * 26 + '(empty)')
            for entry in entries:
                print(' ' * 26 + os.fspath(entry))
