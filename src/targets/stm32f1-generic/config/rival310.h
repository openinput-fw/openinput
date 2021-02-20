/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include "platform/stm32f1/gpio.h"
#include "platform/stm32f1/rcc.h"

#define STM32F103x6

#define EXTERNAL_CLOCK_VALUE STM32F1_CLOCK_12MHZ

static const struct gpio_rgb_led_pin_t leds[] = {
	{
		/* logo led */
		{GPIO_PORT_A, 9},
		{GPIO_PORT_A, 8},
		{GPIO_PORT_A, 10},
	},
	{
		/* scroll led */
		{GPIO_PORT_A, 1},
		{GPIO_PORT_A, 0},
		{GPIO_PORT_A, 2},
	},
};
