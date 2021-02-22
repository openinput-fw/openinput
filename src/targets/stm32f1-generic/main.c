/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/flash.h"
#include "platform/stm32f1/rcc.h"

int main()
{
	flash_latency_config(72000000);

	rcc_init(EXTERNAL_CLOCK_VALUE);
	rcc_update_clock_tree();
}
