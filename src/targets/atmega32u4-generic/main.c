/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "util/data.h"
#include "util/types.h"

#include "platform/avr/boot.h"
#include "platform/avr/usb.h"

void main()
{
	/* Disable watch dog */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

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
