/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "platform/samx7x/hal/spi.h"

void qspi_hal_select(struct spi_hal_t interface, u8 state)
{
	struct qspi_device_t *drv_data = interface.drv_data;
	return qspi_select(*drv_data, state);
}

u8 qspi_hal_transfer(struct spi_hal_t interface, u8 data)
{
	struct qspi_device_t *drv_data = interface.drv_data;
	return qspi_transfer_byte(data);
}

struct spi_hal_t spi_hal_init_qspi(struct qspi_device_t *drv_data)
{
	struct spi_hal_t hal = {
		.transfer = qspi_hal_transfer,
		.select = qspi_hal_select,
		.drv_data = drv_data,
	};
	return hal;
}
