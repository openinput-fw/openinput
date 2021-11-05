/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "platform/samx7x/hal/ticks.h"
#include "platform/samx7x/systick.h"

void ticks_hal_delay_ms(u32 ticks)
{
	return delay_ms(ticks);
}

void ticks_hal_delay_us(u32 ticks)
{
	return delay_us(ticks);
}

struct ticks_hal_t ticks_hal_init()
{
	struct ticks_hal_t hal = {
		.delay_ms = ticks_hal_delay_ms,
		.delay_us = ticks_hal_delay_us,
		.drv_data = NULL,
	};
	return hal;
}
