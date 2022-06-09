/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <string.h>

#include "util/data.h"
#include "util/types.h"

#include "platform/avr/boot.h"
#include "platform/avr/hal/hid.h"
#include "platform/avr/usb.h"

void main()
{
	/* Disable watch dog */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	struct hid_hal_t hid_hal;
	u8 info_functions[] = {
		OI_FUNCTION_VERSION,
		OI_FUNCTION_FW_INFO,
		OI_FUNCTION_SUPPORTED_FUNCTION_PAGES,
		OI_FUNCTION_SUPPORTED_FUNCTIONS,
	};

	/* create protocol config */
	struct protocol_config_t protocol_config;
	memset(&protocol_config, 0, sizeof(protocol_config));
	protocol_config.device_name = "openinput Device";
	protocol_config.hid_hal = hid_hal_init();
	protocol_config.functions[INFO] = info_functions;
	protocol_config.functions_size[INFO] = sizeof(info_functions);

	usb_attach_protocol_config(protocol_config);

	/* Initialize USB Subsystem */
	usb_init();

	/* Enable global interrupts */
	sei();

	u8 led_state = 0;

	for (;;) {
		usb_task();
	}

	bootloader();
}
