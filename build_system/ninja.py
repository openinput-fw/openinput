# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import os
import pathlib
import typing

from typing import Any, List, Optional, Union

import ninja_syntax


class Writer(ninja_syntax.Writer):  # type: ignore
    def __init__(
        self,
        *args: Any,
        toolchain: Optional[str],
        **kwargs: Any
    ) -> None:
        super().__init__(*args, **kwargs)
        self._toolchain = toolchain

    # helpers

    src_dir = os.path.join('$root', 'src')

    @classmethod
    def src(cls, *files: str) -> str:
        return os.path.join(cls.src_dir, *files)

    @classmethod
    def root(cls, *files: str) -> str:
        return os.path.join('$root', *files)

    @classmethod
    def built(cls, file: Union[str, pathlib.Path]) -> str:
        return os.path.join('$builddir', str(file))

    @classmethod
    def remove_ext(cls, file: str) -> str:
        return file.split('.')[0]

    @classmethod
    def extension(cls, file: str, ext: str) -> str:
        return cls.built(os.path.join(
            'out', '.'.join(filter(None, [file, ext]))
        ))

    def cc(self, file: str, **kwargs: Any) -> List[str]:
        return typing.cast(List[str], self.build(
            self.built(f'{self.remove_ext(file)}.o'),
            'cc',
            self.src(file),
            **kwargs,
        ))

    def cc_abs(self, file: str, **kwargs: Any) -> List[str]:
        return self.build(
            self.built(f'{self.remove_ext(file)}.o'),
            'cc',
            self.root(file),
            **kwargs,
        )

    def tool(self, name: str) -> str:
        return '-'.join(filter(None, [self._toolchain, name]))
