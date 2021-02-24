/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#pragma once

#include "hal/spi.h"
#include "platform/stm32f1/spi.h"
#include "util/types.h"

struct spi_hal_t spi_hal_init(struct spi_device_t *drv_data);
u8 spi_hal_transfer(struct spi_hal_t interface, u8 data);
void spi_hal_select(struct spi_hal_t interface, u8 state);
