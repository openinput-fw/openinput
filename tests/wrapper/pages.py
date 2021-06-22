# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

from typing import List, NamedTuple, Optional, Set


class Function(NamedTuple):
    '''openinput protocol function identifier'''
    page_id: int
    function_id: int


class _PageMeta(type):
    '''
    Metaclass that takes an ``id`` argument (function page id) and transform
    the declared fields into :py:class:`Function` instances.

    Example::

        class SomePage(..., id=0x5):
            FUNC1 = 0x01
            FUNC2 = 0x02
            FUNC3 = 0x03

        assert SomePage._PAGE_ID == 0x05
        assert SomePage.FUNC1 == Function(0x05, 0x01)
        assert SomePage.FUNC2 == Function(0x05, 0x02)
        assert SomePage.FUNC3 == Function(0x05, 0x03)
    '''
    def __new__(mcs, name, bases, dict_, id: Optional[int] = None):
        if len(bases) != 0:  # not the base class - _Page
            if id is None:
                raise ValueError('Missing `id` class construction argument')
            assert isinstance(id, int)
            for attr in dict_:
                if not attr.startswith('_'):
                    dict_[attr] = Function(id, dict_[attr])
            dict_['_PAGE_ID'] = id
        return super().__new__(mcs, name, bases, dict_)


# Function pages


class _Page(metaclass=_PageMeta):
    '''Function page base class.'''
    _PAGE_ID: int


class Info(_Page, id=0x00):
    VERSION = 0x01
    FW_INFO = 0x02
    SUPPORTED_FUNCTION_PAGES = 0x03
    SUPPORTED_FUNCTIONS = 0x04


class GeneralProfiles(_Page, id=0x01):
    pass


class Gimmicks(_Page, id=0xFD):
    pass


class Debug(_Page, id=0xFE):
    pass


# Firmware internals

# enum supported_pages_index
PAGE_INDEXES = [
    Info,
    Gimmicks,
    Debug,
]


def fw_page_index_from_id(id: int) -> int:
    '''Converts a function page ID to the internal page index in the firmware.'''
    for i, page in enumerate(PAGE_INDEXES):
        if page._PAGE_ID == id:
            return i
    raise KeyError('Page ID not found')


def functions_to_fw_page_array(functions: Set[Function]) -> List[List[int]]:
    '''Converts a set of functions to the firmware functions array (protocol_config_t.functions)'''
    page_list: List[List[int]] = [[] for _ in range(len(PAGE_INDEXES))]
    for function in functions:
        try:
            page_index = fw_page_index_from_id(function.page_id)
        except KeyError:
            continue
        page_list[page_index].append(function.function_id)
    return page_list
