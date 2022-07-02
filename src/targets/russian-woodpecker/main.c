/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include "util/data.h"
#include "util/types.h"

#include "platform/sam3u/eefc.h"
#include "platform/sam3u/pmc.h"
#include "platform/sam3u/wdt.h"

void main()
{
	pmc_init(EXTERNAL_CLOCK_VALUE, 0UL);
	pmc_update_clock_tree();

	wdt_disable();

	// systick_init();

	for (;;) {
	}
}
