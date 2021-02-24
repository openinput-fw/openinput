/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#pragma once

#include "util/types.h"

struct spi_hal_t {
	u8 (*transfer)(struct spi_hal_t interface, u8 data);
	void (*select)(struct spi_hal_t interface, u8 state);
	void *drv_data;
};
