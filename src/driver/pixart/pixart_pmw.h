/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "hal/spi.h"
#include "hal/ticks.h"
#include "util/types.h"

struct deltas_t {
	s16 dx;
	s16 dy;
};

struct pixart_pmw_driver_t {
	u8 pid;
	u8 motion_flag;
	struct deltas_t deltas;
	struct spi_hal_t spi_hal;
	struct ticks_hal_t ticks_hal;
};

struct pixart_pmw_driver_t pixart_pmw_init(u8 *firmware, struct spi_hal_t spi_hal, struct ticks_hal_t ticks_hal);

void pixart_pmw_read_motion(struct pixart_pmw_driver_t *driver);

void pixart_pmw_motion_event(struct pixart_pmw_driver_t *driver);

struct deltas_t pixart_pmw_get_deltas(struct pixart_pmw_driver_t *driver);

void pixart_pmw_set_cpi(struct pixart_pmw_driver_t driver, u16 cpi);
