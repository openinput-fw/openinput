#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import os
import pathlib
import traceback

from typing import Dict

import build_system
import build_system.builder
import build_system.dependencies
import build_system.ninja
import configure


class OSSFuzzNinjaBuilder(build_system.ninja.NinjaBuilder):
    """ninja build for oss-fuzz.

    Uses $CXX to link and injects $CFLAGS, $CXXFLAGS and $LIB_FUZZING_ENGINE.
    https://google.github.io/oss-fuzz/getting-started/new-project-guide/#Requirements
    """

    def write_variables(
        self,
        details: build_system.BuildDetails,
        settings: build_system.BuildSettings,
        dependencies: Dict[str, build_system.dependencies.Dependency],
    ) -> None:
        self.writer.comment('variables')
        self.writer.newline()

        # paths
        for placeholder, path in self._path_placeholders.items():
            self.writer.variable(placeholder, path)

        # tools
        self.writer.variable('cc', os.environ.get('CC', 'clang'))
        self.writer.variable('cxx', os.environ.get('CXX', 'clang++'))
        self.writer.variable('ar', 'ar')
        self.writer.variable('objcopy', 'objcopy')
        self.writer.variable('size', 'size')

        # flags
        self.writer.variable('c_flags', details.c_flags + [
            os.environ.get('CFLAGS', ''),
        ])
        self.writer.variable('cxx_flags', details.c_flags + [
            os.environ.get('CXXFLAGS', ''),
        ])
        self.writer.variable('c_include_flags', [
            f'-I{self.path(self._location.code)}'
        ] + [
            f'-include {self.path(path)}' for path in details.include_files
        ] + [
            f'-I{self.path(path)}'
            for dep in dependencies.values()
            for path in dep.external_include
        ])
        self.writer.variable('ld_flags', details.ld_flags + ([
            f'-L{self.path(details.linker_dir)}',
            f'-T{details.linker.relative_to(details.linker_dir)}',
        ] if details.linker else []) + [
            os.environ.get('LIB_FUZZING_ENGINE', '-fsanitize=fuzzer,address'),
        ])

        self.writer.newline()

    def write_rules(self) -> None:
        self.writer.comment('rules')
        self.writer.newline()
        self.writer.rule(
            'cc',
            command='$cc -MMD -MT $out -MF $out.d $c_flags $c_include_flags -c $in -o $out',
            depfile='$out.d',
            deps='gcc',
            description='CC $out',
        )
        self.writer.rule(
            'cxx',
            command='$cc -MMD -MT $out -MF $out.d $cxx_flags $c_include_flags -c $in -o $out',
            depfile='$out.d',
            deps='gcc',
            description='CXX $out',
        )
        self.writer.newline()
        self.writer.rule(
            'ar',
            command='rm -f $out && $ar crs $out $in',
            description='AR $out',
        )
        self.writer.newline()
        self.writer.rule(
            'link',
            command='$cxx -o $out $in $ld_flags',
            description='LINK $out',
        )
        self.writer.newline()
        self.writer.rule(
            'bin',
            command='$objcopy -O binary $in $out',
            description='BIN $out',
        )
        self.writer.newline()
        self.writer.rule(
            'hex',
            command='$objcopy -O ihex $in $out',
            description='HEX $out',
        )
        self.writer.newline()


if __name__ == '__main__':
    project_root = pathlib.Path(__file__).absolute().parent
    cc = os.environ.get('CC', 'clang')
    if cc not in ('gcc', 'clang'):
        raise ValueError('Incompatible $CC, only `gcc` or `clang` are supported')
    try:
        builder = build_system.builder.Builder(
            build_system.BuildLocation(project_root, 'build-fuzz'),
            build_system.TargetInfo('fuzz'),
            build_system.BuildSettings(compiler=cc),  # type: ignore[arg-type]
            build_system.VendorInfo(),
            build_system.VersionInfo.from_git(),
        )
        builder.print_summary()
        builder.write_ninja(ninja_build_cls=OSSFuzzNinjaBuilder)
        print("\nbuild.ninja written! Call 'ninja' to compile...")
    except Exception as e:
        print()
        print(configure._DIM + traceback.format_exc() + configure._RESET)
        configure._error(str(e))
