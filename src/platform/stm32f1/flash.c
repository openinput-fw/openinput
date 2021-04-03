/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/flash.h"

void flash_latency_config(u32 clock)
{
	FLASH->ACR &= ~FLASH_ACR_LATENCY;

	if (clock <= 24000000) {
		FLASH->ACR |= 0; /* 0 wait states */
	} else if (clock <= 48000000) {
		FLASH->ACR |= 1; /* 1 wait state */
	} else {
		FLASH->ACR |= 2; /* 2 wait states */
	}
}
