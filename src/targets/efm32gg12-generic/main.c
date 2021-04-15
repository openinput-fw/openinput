/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <em_device.h>

#include "util/data.h"
#include "util/types.h"

#include "platform/efm32gg/cmu.h"
#include "platform/efm32gg/emu.h"
#include "platform/efm32gg/gpio.h"
#include "platform/efm32gg/systick.h"
#include "platform/efm32gg/usb.h"

#define CFG_TUSB_CONFIG_FILE "targets/efm32gg12-generic/tusb_config.h"
#include "tusb.h"

int main()
{
#if defined(DCDC_PRESENT) && defined(DCDC_ENABLE)
	emu_dcdc_init(DCDC_VOLTAGE, DCDC_ACTIVE_I, DCDC_SLEEP_I, DCDC_REVERSE_I); // Init DC-DC converter
	emu_init(0);
#else
	emu_init(1);
#endif

	emu_reg_init(REGULATOR_OUT); // set internal regulator voltage

	cmu_hfxo_startup_calib(0x200, 0x145); // Config HFXO Startup for 1280 uA, 36 pF (18 pF + 2 pF CLOAD)
	cmu_hfxo_steady_calib(0x009, 0x145); // Config HFXO Steady for 12 uA, 36 pF (18 pF + 2 pF CLOAD)

	cmu_init(EXTERNAL_CLOCK_VALUE); // Init Clock Management Unit
	cmu_update_clock_tree();

	cmu_ushfrco_calib(1, USHFRCO_CALIB_48M, 48000000); // Enable and calibrate USHFRCO for 48 MHz
	cmu_auxhfrco_calib(1, AUXHFRCO_CALIB_32M, 32000000); // Enable and calibrate AUXHFRCO for 32 MHz

	systick_init(); // Init system tick

	struct gpio_config_t gpio_config;

	struct gpio_pin_t led_io = {.port = GPIO_PORT_A, .pin = 12};

	gpio_init_config(&gpio_config);

	gpio_setup_pin(&gpio_config, led_io, GPIO_MODE_WIREDAND, 0);

	gpio_apply_config(gpio_config);

	usb_init();

	for (;;) {
		tud_task();
	}
}
