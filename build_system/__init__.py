# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from __future__ import annotations

import dataclasses
import functools
import os
import pathlib
import subprocess
import sys

from typing import Any, Dict, Iterable, List, Literal, Mapping, Optional, Set

import tomli


@dataclasses.dataclass(init=False)
class BuildLocation:
    """Build location data.

    Depends on the current working directory!
    """

    source: pathlib.Path
    build: pathlib.Path

    def __init__(self, source: os.PathLike[str], build: os.PathLike[str]):
        self.source = self._relative_path_builder(source)
        self.build = self._relative_path_builder(build)

    @staticmethod
    def _relative_path_builder(path: os.PathLike[str]) -> pathlib.Path:
        path = pathlib.Path(path).absolute()
        try:
            return path.relative_to(os.getcwd())
        except ValueError:
            return path

    @property
    def code(self) -> pathlib.Path:
        return self.source / 'src'

    @property
    def linkers(self) -> pathlib.Path:
        return self.source / 'config' / 'linker'

    @property
    def targets(self) -> pathlib.Path:
        return self.source / 'config' / 'targets'

    @property
    def families(self) -> pathlib.Path:
        return self.source / 'config' / 'families'

    @property
    def base_config(self) -> pathlib.Path:
        return self.source / 'config' / 'base.toml'

    def source_files(self, files: Optional[Iterable[str]]) -> Iterable[pathlib.Path]:
        if not files:
            return
        for file in files:
            yield self.code / file

    def target_source_files(
        self,
        config: TargetConfig,
        files: Optional[Iterable[str]],
    ) -> Iterable[pathlib.Path]:
        if not files:
            return
        base = self.code / 'targets' / config.target_name
        for file in files:
            yield base / file

    def family_source_files(
        self,
        config: TargetConfig,
        files: Optional[Iterable[str]],
    ) -> Iterable[pathlib.Path]:
        if not files:
            return
        assert config.family_name
        base = self.code / 'platform' / config.family_name
        for file in files:
            yield base / file


@dataclasses.dataclass
class TargetInfo:
    """Information describing the target we want to build."""
    name: str
    config: Optional[str] = None
    type: Literal['firmware', 'bootloader'] = 'firmware'


@dataclasses.dataclass
class BuildSettings:
    """Settings to customize the build."""
    mode: Literal['debug', 'release'] = 'release'
    toolchain: Optional[str] = None
    compiler: Literal['gcc', 'clang'] = 'gcc'


@dataclasses.dataclass
class VendorInfo:
    """Vendor information for the build."""
    name: str = 'openinput-git'
    vid: int = 0x1d50
    pid: int = 0x616a


@dataclasses.dataclass
class VersionInfo:
    """Version information for the build."""
    string: str
    full_string: str  # version with .dirty suffix
    dirty: Optional[bool] = None

    @classmethod
    def from_git(cls) -> VersionInfo:
        version = cls._calculate_git_version()
        dirty = cls._is_git_dirty()
        return cls(
            version,
            f'{version}.dirty' if dirty else version,
            dirty,
        )

    @staticmethod
    def _calculate_git_version() -> str:
        try:
            tag_commit = subprocess.check_output(['git', 'rev-list', '--tags', '--max-count=1']).decode().strip()
            if tag_commit:
                # if there is a tag, use git describe
                return subprocess.check_output(['git', 'describe']).decode().strip()
            # fallback to r{commit count}.{shortened commit id}
            return 'r{}.{}'.format(
                subprocess.check_output(['git', 'rev-list', '--count', 'HEAD']).decode().strip(),
                subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode().strip(),
            )
        except FileNotFoundError:
            return 'UNKNOWN'

    @staticmethod
    def _is_git_dirty() -> Optional[bool]:
        try:
            return bool(subprocess.check_output(['git', 'diff', '--stat']).decode().strip())
        except FileNotFoundError:
            return None


class ConfigLoader():
    """Object responsible for finding the configs the the project source."""

    def __init__(self, location: BuildLocation) -> None:
        self._location = location
        self._src_targets = location.source / 'src' / 'targets'

    def get_config(self, target: TargetInfo) -> TargetConfig:
        return TargetConfig(self, target)

    def get_base_config_path(self) -> pathlib.Path:
        if not self._location.base_config.is_file():
            raise ValueError('Could not find base config')
        return self._location.base_config

    def get_target_config_path(self, target: TargetInfo) -> pathlib.Path:
        if target.name not in self.available_targets:
            raise ValueError(f'Could not find config for target `{target.name}`')
        return self._location.targets / f'{target.name}.toml'

    def get_family_config_path(self, family: str) -> pathlib.Path:
        if family not in self.available_families:
            raise ValueError(f'Could not find config for family `{family}`')
        return self._location.families / f'{family}.toml'

    @functools.cached_property
    def available_targets(self) -> Iterable[str]:
        """Available target names."""
        return [
            target.stem
            for target in self._location.targets.iterdir()
            if target.suffix == '.toml'
        ]

    @functools.cached_property
    def available_families(self) -> Iterable[str]:
        """Available family names."""
        return [
            target.stem
            for target in self._location.families.iterdir()
            if target.suffix == '.toml'
        ]

    @functools.cached_property
    def available_target_configs(self) -> Mapping[str, Iterable[str]]:
        """Mapping from the targets to their available sub-configs (target customization)."""
        configs: Dict[str, List[str]] = {}
        for target in self.available_targets:
            config_file = self._location.targets / f'{target}.toml'
            config = tomli.loads(config_file.read_text())
            if 'has-config' in config:
                config_dir = self._src_targets / target / 'config'
                configs[target] = [
                    file.stem
                    for file in config_dir.iterdir()
                    if file.suffix == '.h'
                ]
        return configs


class TargetConfig():
    """Object that loads the configs for a target."""

    def __init__(self, loader: ConfigLoader, target: TargetInfo) -> None:
        self._loader = loader
        self._target = target

    @property
    def target_name(self) -> str:
        return self._target.name

    @property
    def family_name(self) -> Optional[str]:
        return self.target.get('family')

    @property
    def name(self) -> str:
        assert self._target.config
        return self._target.config

    @functools.cached_property
    def base(self) -> Mapping[str, Any]:
        config_path = self._loader.get_base_config_path()
        return tomli.loads(config_path.read_text())

    @functools.cached_property
    def target(self) -> Mapping[str, Any]:
        # XXX: don't hardcode this
        if self._target.name == 'linux-uhid' and sys.platform != 'linux':
            raise ValueError('linux-uhid target is only available on Linux')

        config_path = self._loader.get_target_config_path(self._target)
        return tomli.loads(config_path.read_text())

    @functools.cached_property
    def family(self) -> Mapping[str, Any]:
        if 'family' not in self.target:
            return {}

        family = self.target['family']
        if not family:
            raise ValueError('Target family was specified but is empty')

        config_path = self._loader.get_family_config_path(family)
        return tomli.loads(config_path.read_text())


@dataclasses.dataclass(init=False)
class BuildDetails:
    """Information detailing the build we want to perform."""

    source: Set[pathlib.Path]
    c_flags: List[str]
    ld_flags: List[str]
    include_files: List[pathlib.Path]
    linker_dir: pathlib.Path
    linker: Optional[pathlib.Path] = None

    def __init__(
        self,
        target: TargetInfo,
        config: TargetConfig,
        settings: BuildSettings,
        location: BuildLocation,
        vendor: VendorInfo,
        version: VersionInfo,
    ) -> None:
        self.source = set()
        self.c_flags = []
        self.ld_flags = []
        self.include_files = []
        self.linker_dir = location.linkers

        target_path = location.code / 'targets' / config.target_name
        if target.type == 'firmware':
            self.source.add(target_path / 'main.c')
        elif target.type == 'bootloader':
            self.source.add(target_path / 'bootloader.c')

        if config.family.get('has-linker', True) is True:
            if config.target.get('has-config', False) is True:
                # if the target has config, each config gets it own linker
                self.linker = location.linkers / config.target_name / f'{config.name}.ld'
            else:
                # otherwise, it has a single linker in the common directory
                self.linker = location.linkers / f'{config.target_name}.ld'

        self.source.update(location.source_files(config.base.get('source')))
        self.c_flags += config.base.get('c_flags', [])
        self.ld_flags += config.base.get('ld_flags', [])
        self.include_files += location.source_files(config.base.get('include_files'))

        for source_mapper, data in (
            (location.family_source_files, config.family),
            (location.family_source_files, config.family.get(settings.mode, {})),  # debug/release
            (location.target_source_files, config.target),
            (location.target_source_files, config.target.get(settings.mode, {})),
        ):
            self.source.update(source_mapper(config, data.get('source')))
            self.c_flags += data.get('c_flags', [])
            self.ld_flags += data.get('ld_flags', [])
            self.include_files += source_mapper(config, data.get('include_files'))

        if target.config:
            self.include_files.append(
                location.code / 'targets' / target.name / 'config' / f'{target.config}.h'
            )

        self.c_flags += [
            fr'-DOI_VENDOR=\"{vendor.name}\"',
            fr'-DOI_VERSION=\"{version.full_string}\"',
        ]
