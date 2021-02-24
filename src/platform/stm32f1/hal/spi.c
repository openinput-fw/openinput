/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#include "platform/stm32f1/hal/spi.h"

struct spi_hal_t spi_hal_init(struct spi_drv_data_t *drv_data)
{
	struct spi_hal_t hal = {
		.transfer = spi_hal_transfer,
		.select = spi_hal_select,
		.drv_data = drv_data,
	};
	return hal;
}

u8 spi_hal_transfer(struct spi_hal_t interface, u8 data)
{
	struct spi_drv_data_t *drv_data = interface.drv_data;
	return spi_transfer_byte(*drv_data, data);
}

void spi_hal_select(struct spi_hal_t interface, u8 state)
{
	struct spi_drv_data_t *drv_data = interface.drv_data;
	return spi_select(*drv_data, state);
}
