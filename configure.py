#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from __future__ import annotations

import argparse
import functools
import os
import shutil
import subprocess
import sys
import traceback
import warnings

from typing import Any, Dict, List, MutableMapping, Optional, Set, TextIO, Type, Union


_MIN_PYTHON_VERSION = (3, 9)


# disable colors if we're not in a TTY
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


def _error(msg: str, code: int = 1) -> None:
    '''
    Prints an error message and exit with code
    '''
    print(f'{_RED}ERROR{_RESET} {msg}')
    exit(code)


if sys.version_info < _MIN_PYTHON_VERSION:
    _error(
        f'Unsupported Python version: {".".join(map(str, sys.version_info[:3]))}... '
        f'You need at least Python {".".join(map(str, _MIN_PYTHON_VERSION))}.'
    )


# external imports with nice error messages for better UX

try:
    import toml
except ImportError:
    _error(
        'Missing toml dependency!\n'
        'You can install it with: pip install toml'
    )

try:
    import ninja_syntax  # noqa: F401
except ImportError:
    _error(
        'Missing ninja_syntax dependency!\n'
        'You can install it with: pip install ninja_syntax'
    )
else:
    import build_system.dependencies
    import build_system.ninja


# overide default warning handler for a nice CLI

def _showwarning(
    message: Union[Warning, str],
    category: Type[Warning],
    filename: str,
    lineno: int,
    file: Optional[TextIO] = None,
    line: Optional[str] = None,
) -> None:
    '''
    Show warning to the console

    We override warnings.showwarning with this to match our CLI
    '''
    prefix = 'WARNING'
    if sys.stdout.isatty():
        prefix = '\33[93m' + prefix + '\33[0m'
    print('{} {}'.format(prefix, str(message)))


warnings.showwarning = _showwarning


# find available targets
_root = os.path.abspath(os.path.dirname(__file__))
_configs = os.path.join(_root, 'config')
_configs_targets = os.path.join(_configs, 'targets')
_configs_families = os.path.join(_configs, 'families')
TARGETS = [
    # look for targets in config/targets
    target.split('.')[0] for target in os.listdir(_configs_targets)
    if target.endswith('.toml')
]

# find target config options, for the ones that support configs
TARGET_CONFIGS: Dict[str, List[str]] = {}
for _target in TARGETS:
    # find the targets that have `has-config` in their config, and populate the
    # possible config choices with the options available in src/targets/{_target}/config
    _target_config = toml.load(os.path.join(_configs_targets, f'{_target}.toml'))
    if 'has-config' in _target_config:
        _configs_dir = os.path.join(_root, 'src', 'targets', _target, 'config')
        TARGET_CONFIGS[_target] = [
            config.split('.')[0] for config in os.listdir(_configs_dir)
            if config.endswith('.h')
        ]


class BuildError(Exception):
    pass


class BuildSystemBuilder():
    _REQUIRED_TOOLS = (
        'ar',
        'gcc',
        'objcopy',
        'size',
    )

    _target_config: MutableMapping[str, Any]
    _family_config: MutableMapping[str, Any]

    def __init__(
        self,
        target: str,
        *,
        builddir: str = 'build',
        debug: bool = False,
        toolchain: Optional[str] = None,
        vendor: str = 'openinput-git',
        config: Optional[str] = None,
        no_fetch: bool = False,
    ) -> None:
        self._target = target
        self._toolchain = toolchain
        self._vendor = vendor
        self._config = config
        self._no_fetch = no_fetch

        self._build_mode = 'debug' if debug else 'release'

        self._this_file_name = os.path.basename(__file__)
        self._linker: Optional[str] = None

        self._calculate_root(builddir)
        self._load_target_config()
        self._load_family_config()
        self._load_base_config()

        self._populate_source()
        self._populate_linker()
        self._populate_options()
        self._populate_vendor_options()

        self._validate_toolchain()
        self._calculate_extensions()

        self._populate_dependencies()

    def _calculate_root(self, builddir: str) -> None:
        self._root = _root
        if self._root == os.getcwd():  # use relative path if we are building in the current directory
            self._root = os.curdir

        self._builddir = os.path.join(builddir, self._target)

    def _load_target_config(self) -> None:
        if not self._target:
            raise ValueError('Invalid target: empty target')
        if self._target == 'linux-uhid' and sys.platform != 'linux':
            raise ValueError('linux-uhid target is only available on Linux')

        config_path = os.path.join(_configs_targets, f'{self._target}.toml')
        if not os.path.isfile(config_path):
            raise ValueError(f'Target not found: {self._target} ({config_path})')

        self._target_config = toml.load(config_path)

    def _load_family_config(self) -> None:
        if 'family' not in self._target_config:
            if not self._target_config['family']:
                raise ValueError('Target family was specified but is empty')
            self._family_config = {}
            return

        self._family = self._target_config['family']
        config_path = os.path.join(_configs_families, f'{self._family}.toml')
        if not os.path.isfile(config_path):
            raise ValueError(f'Family not found: {self._target_config[self._family]} ({config_path})')

        self._family_config = toml.load(config_path)

    def _load_base_config(self) -> None:
        config_path = os.path.join(_configs, 'base.toml')
        if not os.path.isfile(config_path):
            raise ValueError(f'Base config not found ({config_path})')

        self._base_config = toml.load(config_path)

    def _populate_source(self) -> None:
        self._source: Set[str] = set()

        # common
        base_src = self._base_config.get('source', [])

        # family
        family_src = self._family_config.get('source', [])
        try:
            family_src += self._family_config[self._build_mode]['source']
        except KeyError:
            pass

        # target
        target_src = self._target_config.get('source', [])
        try:
            target_src += self._target_config[self._build_mode]['source']
        except KeyError:
            pass

        # TODO: select between main.c and bootloader.c
        target_src.append('main.c')

        self._source.update([
            file.replace('/', os.path.sep) for file in base_src
        ] + [
            os.path.join('platform', self._family, file) for file in family_src
        ] + [
            os.path.join('targets', self._target, file) for file in target_src
        ])

    def _populate_linker(self) -> None:
        if self._family_config.get('has-linker', True) is False:  # default to true
            return  # does not need linker

        self._linker_dir = os.path.join('config', 'linker')

        # if the target has config, each config gets it own linker
        if self._target_config.get('has-config', False) is True:
            self._linker = '{}.ld'.format(os.path.join(self._target, self._config))
        else:  # otherwise, it has a single linker in the common directory
            self._linker = f'{self._target}.ld'

        linker_path = os.path.abspath(os.path.join(__file__, '..', self._linker_dir, self._linker))
        if not os.path.isfile(linker_path):
            raise FileNotFoundError(f'Linker script not found: {linker_path}')

    def _populate_options(self) -> None:
        self._options: Dict[str, List[str]] = {
            'c_flags': [],
            'ld_flags': [],
            'include_files': [],
        }

        # populate c_flags and ld_flags
        for config in (self._target_config, self._family_config):
            for option in ('c_flags', 'ld_flags'):
                self._options[option] += config.get(option, [])
                try:
                    self._options[option] += config[self._build_mode][option]
                except KeyError:
                    pass

        # target has a config
        if self._target_config.get('has-config', False) is True:
            if not self._config:
                raise ValueError('Was expecting a target config but none specified')
            self._options['include_files'] += [
                os.path.join('targets', self._target, 'config', f'{self._config}.h')
            ]

    def _populate_vendor_options(self) -> None:
        self._options['c_flags'] += [
            fr'-DOI_VENDOR=\"{self._vendor}\"',
            r'-DOI_VERSION=\"{}\"'.format(
                f'{self.version}.dirty' if self._is_git_dirty() else self.version
            ),
        ]

    def _validate_toolchain(self) -> None:
        if self._toolchain is not None:  # override toolchain
            warnings.warn('Overriding target toolchain, may not be compatible!')
        else:  # use the toolchain defined by the target
            self._toolchain = self._target_config.get(
                'toolchain',
                self._family_config.get('toolchain'),
            )

        if self._toolchain:
            if self._target_config.get('is-shared-library'):
                _error('`is-shared-library` not available when cross compiling')
            if self._toolchain.endswith('-'):
                warnings.warn(
                    f'Specified `{self._toolchain}` as the cross toolchain, '
                    f'did you mean `{self._toolchain[:-1]}`?'
                )
        for tool in self._REQUIRED_TOOLS:
            name = '-'.join(filter(None, [self._toolchain, tool]))
            if shutil.which(name) is None:
                _error(f'Invalid toolchain: {name} not found')

    def _calculate_extensions(self) -> None:
        if not self._toolchain:  # native
            if self._target_config.get('is-shared-library'):
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
        self._dependencies: Dict[str, build_system.dependencies.Dependency] = {}

        dependencies = self._family_config.get('dependencies', {})
        dependencies.update(
            self._target_config.get('dependencies', {})
        )

        try:
            for name, kwargs in dependencies.items():
                cls = build_system.dependencies.Dependency.class_from_name(name)
                if not self._no_fetch:
                    cls.fetch_submodule()
                self._dependencies[name] = cls(**kwargs)
        except TypeError as e:
            raise BuildError(f'Failed to initialize depedency `{name}`: {str(e)}')

    def _ninja_write_header(self, nw: build_system.ninja.Writer) -> None:
        nw.comment(f'build file automatically generated by {self._this_file_name}, do not edit manually!')
        nw.newline()
        nw.variable('ninja_required_version', '1.3')
        nw.newline()

    def _ninja_write_variables(self, nw: build_system.ninja.Writer) -> None:
        linker_args = [
            '-L{}'.format(os.path.join('$root', self._linker_dir)),
            f'-T{self._linker}',
        ] if self._linker else []

        nw.comment('variables')
        nw.newline()
        nw.variable('root', self._root)
        nw.variable('builddir', self._builddir)
        nw.variable('cc', nw.tool('gcc'))
        nw.variable('ar', nw.tool('ar'))
        nw.variable('objcopy', nw.tool('objcopy'))
        nw.variable('size', nw.tool('size'))
        nw.variable('c_flags', self._options['c_flags'])
        nw.variable('c_include_flags', ' '.join([
            f'-I{nw.src_dir}',
        ] + [
            f'-include {nw.src(path)}' for path in self._options['include_files']
        ] + [
            f'-I{nw.root(path)}'
            for dependency in self._dependencies.values()
            for path in dependency.external_include
        ]))
        nw.variable('ld_flags', self._options['ld_flags'] + linker_args)
        nw.newline()

    def _ninja_write_rules(self, nw: build_system.ninja.Writer) -> None:
        nw.comment('rules')
        nw.newline()
        nw.rule(
            'cc',
            command='$cc -MMD -MT $out -MF $out.d $c_flags $c_include_flags -c $in -o $out',
            depfile='$out.d',
            deps='gcc',
            description='CC $out',
        )
        nw.newline()
        nw.rule(
            'ar',
            command='rm -f $out && $ar crs $out $in',
            description='AR $out',
        )
        nw.newline()
        nw.rule(
            'link',
            command='$cc -o $out $in $ld_flags',
            description='LINK $out',
        )
        nw.newline()

        if self._generate_bin:
            nw.rule(
                'bin',
                command='$objcopy -O binary $in $out',
                description='BIN $out',
            )
            nw.newline()

        if self._generate_hex:
            nw.rule(
                'hex',
                command='$objcopy -O ihex $in $out',
                description='HEX $out',
            )
            nw.newline()

    def write_ninja(self, file: str = 'build.ninja') -> None:  # noqa: C901
        with open(file, 'w') as f:
            nw = build_system.ninja.Writer(f, toolchain=self._toolchain, width=160)

            self._ninja_write_header(nw)
            self._ninja_write_variables(nw)
            self._ninja_write_rules(nw)

            objs: List[str] = []

            # optional config argument
            config: Optional[str] = None
            if self._config:
                config = nw.src('targets', self._target, 'config', f'{self._config}.h')

            # build dependencies
            for name, dep in self._dependencies.items():
                objs += dep.write_ninja(
                    nw,
                    self._dependencies,
                    nw.src('targets', self._target),
                    config,
                )

            # build our source
            nw.comment('target objects')
            nw.newline()
            for file in self._source:
                objs += nw.cc(file)
            nw.newline()

            # build output objects
            nw.comment('target')
            nw.newline()
            out_name = self._target_config.get('out-name', '-'.join(filter(None, [
                'openinput',
                self._target,
                self.version.replace('.', '-'),
                'dirty' if self._is_git_dirty() else None,
            ])))
            out = nw.extension(out_name, self._bin_extension)
            if self._target_config.get('is-shared-library') is True:
                nw.build(out, 'link', objs, variables={
                    'ld_flags': self._options['ld_flags'] + ['-shared']
                })
            else:
                nw.build(out, 'link', objs)
            nw.newline()

            if self._generate_bin:
                nw.build(nw.extension(out_name, 'bin'), 'bin', out)
                nw.newline()
            if self._generate_hex:
                nw.build(nw.extension(out_name, 'hex'), 'hex', out)
                nw.newline()

    def _is_git_dirty(self) -> Optional[bool]:
        try:
            return bool(subprocess.check_output(['git', 'diff', '--stat']).decode().strip())
        except FileNotFoundError:
            return None

    @functools.cached_property
    def version(self) -> str:
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

    def print_summary(self) -> None:
        print(
            f'openinput {_CYAN}{self.version}{_RESET}' +
            (f' {_RED}(dirty){_RESET}' if self._is_git_dirty() else ''),
            end='\n\n'
        )

        for name, var in {
            'target': self._target,
            'toolchain': self._toolchain or 'native',
            'c_flags': ' '.join(self._options['c_flags']) or '(empty)',
            'ld_flags': ' '.join(self._options['ld_flags']) or '(empty)',
            'dependencies': ' '.join([dep for dep in self._dependencies]) or None
        }.items():
            if var:
                print('{:>24}: {}'.format(name, var))

        for name, entries in {
            'source': self._source,
            'include_files': self._options['include_files'],
        }.items():
            print('{:>24}:'.format(name))
            if not entries:
                print(' ' * 26 + '(empty)')
            for entry in entries:
                print(' ' * 26 + entry)


def _ask_target() -> str:
    print('Available targets:')
    for i, available_target in enumerate(TARGETS):
        print(f'{available_target:>32} ({i})')
    while True:
        select = input('Select target (default=linux-uhid): ')
        if not select:
            target = 'linux-uhid'
            break
        try:
            target = TARGETS[int(select)]
            break
        except (ValueError, IndexError):
            print('Invalid option!')
    print()
    return target


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--builddir',
        '-b',
        type=str,
        default='build',
        help='build directory',
    )
    parser.add_argument(
        '--toolchain',
        '-t',
        type=str,
        help='override toolchain prefix (eg. arm-none-eabi)',
    )
    parser.add_argument(
        '--debug',
        '-d',
        action='store_true',
        help='output extra debug information',
    )
    parser.add_argument(
        '--no-fetch',
        '-n',
        action='store_true',
        help="don't fetch dependency git submodules",
    )
    target_subparsers = parser.add_subparsers(
        dest='target',
        help='target firmware',
        metavar='TARGET',
    )

    # register targets and thier arguments
    for target in TARGETS:
        target_parser = target_subparsers.add_parser(target)
        if target in TARGET_CONFIGS:
            target_parser.add_argument(
                '--config',
                '-c',
                type=str,
                metavar='CONFIG',
                choices=TARGET_CONFIGS[target],
                help='device configuration name',
                required=True,
            )

    args = parser.parse_args()

    if not args.target:
        # ask for target if it was not specified
        args = parser.parse_args([_ask_target()])

    try:
        builder = BuildSystemBuilder(**vars(args))
        builder.print_summary()
        builder.write_ninja()
        print("\nbuild.ninja written! Call 'ninja' to compile...")
    except Exception as e:
        print()
        print(_DIM + traceback.format_exc() + _RESET)
        _error(str(e))
