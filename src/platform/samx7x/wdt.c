/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/samx7x/wdt.h"

void wdt_disable()
{
	WDT->WDT_MR = RSWDT_MR_WDDIS_Msk;
}
