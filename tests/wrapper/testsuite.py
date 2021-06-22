# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from typing import Set

import _testsuite
import pages


class Device(_testsuite.Device):
    def __init__(
        self,
        *,
        name: str,
        functions: Set[pages.Function],
    ) -> None:
        super().__init__(
            name=name,
            functions=pages.functions_to_fw_page_array(functions),
        )
