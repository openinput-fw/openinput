/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/sam3u/wdt.h"

void wdt_disable()
{
	WDT->WDT_MR = WDT_MR_WDDIS;
}
