# SPDX-License-Identifier: MIT

import argparse
import collections
import os
import os.path
import platform

from typing import Dict, List, Optional, Sequence, Set, Union


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


class BuildConfiguration():
    name: Optional[str] = None
    platform: Optional[str] = None
    bin_extension: Optional[str] = None
    generate_bin = False
    generate_hex = True

    def __init__(
        self,
        debug: bool = False,
        cross_toolchain: Union[Optional[str], bool] = False,
        linker_script: Optional[str] = None,
    ) -> None:
        # cross_toolchain == None: native
        # cross_toolchain == False: not specified
        if isinstance(cross_toolchain, bool):  # .-.
            raise BuildConfigurationError('No toolchain specified')
        self.cross_toolchain = cross_toolchain

        self.c_flags = [
            '-Os',
        ]
        self.ld_flags = []

        # add linker script to ld_flags
        if linker_script:
            linker_scripts_dir = os.path.join(abs_root, 'build-targets', 'linker')
            self.ld_flags += [
                f'-L{linker_scripts_dir}',
                f'-T{linker_script}',
            ]

        self.source: List[str] = []
        self.include = [
            os.path.join(rel_platform('generic'), 'platform-hid.h')
        ]

    @property
    def main(self) -> str:
        if not self.name:
            raise ValueError('Not a target, there is no main')
        return os.path.join(rel_target(self.name), 'main.c')

    @staticmethod
    def parser_append_group(parser: argparse.ArgumentParser) -> None:
        pass

    def platform_files(self, *files: str) -> List[str]:
        if not self.platform:
            raise TypeError('Trying to get platform files from a non-platform configuration')
        return list(map(
            lambda f: os.path.join(
                rel_platform(self.platform),  # type: ignore
                os.pathsep.join(f.split('/'))
            ),
            files,
        ))

    def target_files(self, *files: str) -> List[str]:
        if not self.name:
            raise TypeError('Trying to get target files from a non-target configuration')
        return list(map(
            lambda f: os.path.join(
                rel_target(self.name),  # type: ignore
                os.pathsep.join(f.split('/'))
            ),
            files,
        ))
