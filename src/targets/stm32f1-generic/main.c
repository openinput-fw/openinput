/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/gpio.h"
#include "platform/stm32f1/rcc.h"
#include "util/data.h"

int main()
{
	struct gpio_config_t gpio_config = {0};

	rcc_init(EXTERNAL_CLOCK_VALUE);

	/* configure gpio pins for leds */
	for (size_t i; i < sizeof_t(leds) - 1; i++)
		gpio_rgb_led_setup(&gpio_config, leds[i]);

	gpio_apply_config(gpio_config);
}
