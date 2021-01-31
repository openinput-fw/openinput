#!/usr/bin/env python3
# SPDX-License-Identifier: MIT

import argparse
import functools
import os
import os.path
import shutil
import subprocess
import sys
import traceback
import warnings

from typing import Any, Dict, List, Optional, TextIO, Type, Union

import build_targets
import build_targets.targets


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


class BuildSystemBuilder():
    _REQUIRED_TOOLS = (
        'gcc',
        'objcopy',
        'size',
    )

    def __init__(
        self,
        cli_args: List[str],
        target: str,
        builddir: str = 'build',
        debug: bool = False,
        **kwargs: Any,
    ) -> None:
        self._cli_args = cli_args
        self._debug = debug

        # get target
        if not target:
            raise ValueError('Invalid target: empty target')
        if target == 'linux-uhid' and sys.platform != 'linux':
            raise ValueError('linux-uhid target is only available on Linux')
        if target not in self.get_targets():
            raise ValueError(f'Target not found: {target}')
        self._target = self.get_targets()[target](**kwargs)
        self._validate_toolchain()

        # misc variables

        self._this_file_name = os.path.basename(__file__)

        self._root = os.path.dirname(os.path.realpath(__file__))
        # use relative path if we are building in the current directory
        if self._root == os.getcwd():
            self._root = os.curdir

        self._builddir = builddir

        # construct tool name, eg. arm-none-eabi-gcc, gcc...
        self._tool = lambda x: '-'.join(filter(None, [self._target.toolchain, x]))

    def write_ninja(self, file: str = 'build.ninja') -> None:
        # delayed import to provide better UX
        try:
            import ninja_syntax
        except ImportError:
            _error(
                'Missing ninja_syntax!\n'
                'You can install it with: pip install ninja_syntax'
            )

        with open(file, 'w') as f:
            nw = ninja_syntax.Writer(f)

            # add some information
            nw.comment(f'build file automatically generated by {self._this_file_name}, do not edit manually!')
            nw.newline()
            nw.variable('ninja_required_version', '1.3')
            nw.newline()

            # declare variables
            nw.variable('root', self._root)
            nw.variable('builddir', self._builddir)
            nw.variable('cc', self._tool('gcc'))
            nw.variable('ar', self._tool('ar'))
            nw.variable('objcopy', self._tool('objcopy'))
            nw.variable('size', self._tool('size'))
            nw.variable('c_flags', self._target.c_flags)
            nw.variable('c_include_flags', ' '.join([
                f'-I{path}' for path in self._target.include
            ]))
            nw.variable('ld_flags', self._target.ld_flags)
            nw.newline()

            # helper functions
            src = lambda f: os.path.join('$root', 'src', f)
            built = lambda f: os.path.join('$builddir', f)
            remove_ext = lambda file: file.split('.')[0]
            cc = lambda file, **kwargs: nw.build(
                built(f'{remove_ext(file)}.o'), 'cc', src(file), **kwargs
            )
            extension = lambda file, ext: built(os.path.join(
                'out', '.'.join(filter(None, [file, ext]))
            ))
            binary_name = lambda file: extension(file, self._target.bin_extension)

            # declare rules
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
                command='$cc $ld_flags -o $out $in $libs',
                description='LINK $out',
            )
            nw.newline()

            if self._target.generate_bin:
                nw.rule(
                    'bin',
                    command='$objcopy -O binary $in $out',
                    description='BIN $out',
                )

            if self._target.generate_hex:
                nw.rule(
                    'hex',
                    command='$objcopy -O ihex $in $out',
                    description='HEX $out',
                )

            # compile sources into objects
            objs = []
            for file in set(self._target.source):
                objs += cc(file)
            nw.newline()

            # create executable
            out_name = '-'.join(filter(None, [
                'openinput',
                self._target.name,
                self.version.replace('.', '-'),
                'dirty' if self._is_git_dirty else None,
            ]))
            out = binary_name(out_name)
            nw.build(out, 'link', objs)
            nw.newline()

            # create .bin and .hex
            if self._target.generate_bin:
                nw.build(extension(out_name, 'bin'), 'bin', out)
                nw.newline()
            if self._target.generate_bin:
                nw.build(extension(out_name, 'hex'), 'hex', out)
                nw.newline()

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
        except FileNotFoundError as e:
            print(e)
            return 'UNKNOWN'

    def print_summary(self) -> None:
        print(
            f'openinput {_CYAN}{self.version}{_RESET}' +
            (f' {_RED}(dirty){_RESET}' if self._is_git_dirty() else ''),
            end='\n\n'
        )

        for name, var in {
            'target': self._target.name,
            'toolchain': self._target.toolchain or 'native',
            'c_flags': ' '.join(self._target.c_flags) or '(empty)',
            'ld_flags': ' '.join(self._target.ld_flags) or '(empty)',
        }.items():
            print('{:>24}: {}'.format(name, var))

        if self._debug:
            for name, entries in {
                'source': self._target.source,
                'include': self._target.include,
            }.items():
                print('{:>24}:'.format(name))
                for entry in entries:
                    print(' ' * 26 + entry)

    def _is_git_dirty(self) -> Optional[bool]:
        try:
            return bool(subprocess.check_output(['git', 'diff', '--stat']).decode().strip())
        except FileNotFoundError:
            return None

    def _validate_toolchain(self) -> None:
        prefix = self._target.toolchain
        if prefix and prefix.endswith('-'):
            warnings.warn(f"Specified '{prefix}' as the cross toolchain prefix, did you mean '{prefix[:-1]}'?")
        for tool in self._REQUIRED_TOOLS:
            name = '-'.join(filter(None, [prefix, tool]))
            if shutil.which(name) is None:
                _error(f'Invalid toolchain: {name} not found')

    @staticmethod
    @functools.lru_cache(maxsize=1)
    def get_targets() -> Dict[str, Type[build_targets.BuildConfiguration]]:
        targets_dict: Dict[str, Type[build_targets.BuildConfiguration]] = {}

        def _add(parent_cls: Type[build_targets.BuildConfiguration]) -> None:
            for cls in parent_cls.__subclasses__():
                if cls.name:
                    targets_dict[cls.name] = cls
                _add(cls)

        _add(build_targets.BuildConfiguration)

        return targets_dict


def _ask_target() -> str:
    print('Available targets:')
    targets_list = list(BuildSystemBuilder.get_targets().keys())
    for i, available_target in enumerate(targets_list):
        print(f'{available_target:>32} ({i})')
    while True:
        select = input('Select target (default=linux-uhid): ')
        if not select:
            target = 'linux-uhid'
            break
        try:
            target = targets_list[int(select)]
            break
        except (ValueError, IndexError):
            print('Invalid option!')
    print()
    return target


if __name__ == '__main__':
    if sys.version_info < _MIN_PYTHON_VERSION:
        _error(
            f'Unsupported Python version: {".".join(map(str, sys.version_info[:3]))}... '
            f'You need at least Python {".".join(map(str, _MIN_PYTHON_VERSION))}.'
        )

    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--builddir',
        '-b',
        type=str,
        default='build',
        help="build directory (defaults to 'build/{target}')",
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
    target_subparsers = parser.add_subparsers(
        dest='target',
        help='target firmware',
        metavar='TARGET',
    )

    # register arguments from targets
    for cls in BuildSystemBuilder.get_targets().values():
        assert cls.name
        target_parser = target_subparsers.add_parser(cls.name)
        cls.parser_append_group(target_parser)

    args = parser.parse_args()

    if not args.target:
        # ask for target if it was not specified
        args = parser.parse_args([_ask_target()])

    try:
        builder = BuildSystemBuilder(sys.argv[1:], **vars(args))
        builder.print_summary()
        builder.write_ninja()
        print("\nbuild.ninja written! Call 'ninja' to compile...")
    except Exception as e:
        print()
        print(_DIM + traceback.format_exc() + _RESET)
        _error(str(e))
