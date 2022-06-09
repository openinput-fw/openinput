/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <silvagracarafael@gmail.com>
 */

#include <avr/wdt.h>

// Assuming the Caterina bootloader that comes with the 32u4
uint16_t bootKey = 0x7777;
volatile uint16_t *const bootKeyPtr = (volatile uint16_t *) 0x0800;

void reset()
{
	wdt_enable(WDTO_15MS);
	for (;;) {
	}
}

void bootloader()
{
	*bootKeyPtr = bootKey;

	reset();
}
