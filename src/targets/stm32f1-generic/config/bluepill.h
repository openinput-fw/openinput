/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

/* General Config */

#define STM32F103xB

/* Clock Config */

#define EXTERNAL_CLOCK_VALUE STM32F1_CLOCK_8MHZ

/* USB Config */

#define USB_VID 0x1D50 /* Attributed by openmoko */
#define USB_PID 0x616A /* Attributed by openmoko */

/*
 * The USB standard requires a 1.5k pullup resistor on D+
 * Bluepill boards are known to come with the wrong value resistor here (R10)
 * it is usually populated with a 10k or a 4.7k resistor and should be replaced with a 1.5k
 */
#define USB_DP_PU_IO                           \
	{                                      \
		.port = GPIO_PORT_A, .pin = 12 \
	}
