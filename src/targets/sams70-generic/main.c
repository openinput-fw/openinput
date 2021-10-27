/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "util/data.h"
#include "util/types.h"

#include "platform/samx7x/eefc.h"
#include "platform/samx7x/pio.h"
#include "platform/samx7x/pmc.h"
#include "platform/samx7x/systick.h"
#include "platform/samx7x/usb.h"
#include "platform/samx7x/wdt.h"

#define CFG_TUSB_CONFIG_FILE "targets/sams70-generic/tusb_config.h"
#include "tusb.h"

void main()
{
	wdt_disable();

	eefc_tcm_disable();

	pmc_init(EXTERNAL_CLOCK_VALUE, 0UL);
	pmc_init_usb();
	pmc_update_clock_tree();

	systick_init();

	pio_init();

	usb_init();

	for (;;) {
		tud_task();
	}
}
