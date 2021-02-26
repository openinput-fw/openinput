/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/flash.h"
#include "platform/stm32f1/gpio.h"
#include "platform/stm32f1/rcc.h"
#include "platform/stm32f1/systick.h"
#include "platform/stm32f1/usb.h"

#include "util/data.h"
#include "util/types.h"

#define CFG_TUSB_CONFIG_FILE "targets/stm32f1-generic/tusb_config.h"
#include "tusb.h"

#define OI_MOUSE_REPORT_ID 0x01

struct output_report {
	u8 id;
	s8 x;
	s8 y;
	s8 wheel;
	u8 button1 : 1;
	u8 button2 : 1;
	u8 button3 : 1;
} __attribute__((__packed__));

int main()
{
	flash_latency_config(72000000);

	rcc_init(EXTERNAL_CLOCK_VALUE);

	systick_init();

	struct gpio_config_t gpio_config;
	struct gpio_pin_t usb_dm_io = {.port = GPIO_PORT_A, .pin = 11};
	struct gpio_pin_t usb_dp_io = {.port = GPIO_PORT_A, .pin = 12};
	struct gpio_pin_t usb_dp_pu_io = USB_DP_PU_IO;

	gpio_init_config(&gpio_config);

	gpio_setup_pin(&gpio_config, usb_dm_io, GPIO_MODE_INPUT | GPIO_CNF_INPUT_FLOATING, 0); /* USB DM */
	gpio_setup_pin(&gpio_config, usb_dp_io, GPIO_MODE_INPUT | GPIO_CNF_INPUT_FLOATING, 0); /* USB DP */
	gpio_setup_pin(
		&gpio_config, usb_dp_pu_io, GPIO_MODE_OUTPUT_50MHZ | GPIO_CNF_OUTPUT_GENERAL_PUSH_PULL, 0); /* USB DP PU */

	gpio_apply_config(gpio_config);

	usb_init();

	for (;;) {
		tud_task();
	}
}
