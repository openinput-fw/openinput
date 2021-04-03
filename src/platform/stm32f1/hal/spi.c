/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "platform/stm32f1/hal/spi.h"

void spi_hal_select(struct spi_hal_t interface, u8 state)
{
	struct spi_device_t *drv_data = interface.drv_data;
	return spi_select(*drv_data, state);
}

u8 spi_hal_transfer(struct spi_hal_t interface, u8 data)
{
	struct spi_device_t *drv_data = interface.drv_data;
	return spi_transfer_byte(*drv_data, data);
}

struct spi_hal_t spi_hal_init(struct spi_device_t *drv_data)
{
	struct spi_hal_t hal = {
		.transfer = spi_hal_transfer,
		.select = spi_hal_select,
		.drv_data = drv_data,
	};
	return hal;
}
