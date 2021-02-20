/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

#define min(a, b)   (((a) < (b)) ? (a) : (b))
#define max(a, b)   (((a) > (b)) ? (a) : (b))
#define sizeof_t(a) (sizeof(a) ? sizeof(a) / sizeof(a[0]) : 0)

#define BIT(x) (1 << (x))

#define REG_SET(reg, mask, value) (reg = (reg & ~mask) | value)
