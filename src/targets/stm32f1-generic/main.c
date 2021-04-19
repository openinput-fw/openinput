/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/flash.h"
#include "platform/stm32f1/gpio.h"
#include "platform/stm32f1/hal/hid.h"
#include "platform/stm32f1/hal/spi.h"
#include "platform/stm32f1/hal/ticks.h"
#include "platform/stm32f1/rcc.h"
#include "platform/stm32f1/spi.h"
#include "platform/stm32f1/systick.h"
#include "platform/stm32f1/usb.h"

#include "driver/pixart/pixart_pmw.h"
#include "pixart_blobs.h"

#include "util/data.h"
#include "util/types.h"

#include "protocol/protocol.h"

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

void main()
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

#if defined(SENSOR_ENABLED) && SENSOR_DRIVER == PIXART_PMW
	struct gpio_pin_t sensor_motion_io = SENSOR_MOTION_IO;
	struct gpio_pin_t sensor_cs_io = SENSOR_INTERFACE_CS_IO;
	struct gpio_pin_t sensor_sck_io = SENSOR_INTERFACE_SCK_IO;
	struct gpio_pin_t sensor_miso_io = SENSOR_INTERFACE_MISO_IO;
	struct gpio_pin_t sensor_mosi_io = SENSOR_INTERFACE_MOSI_IO;

	gpio_setup_pin(&gpio_config, sensor_motion_io, GPIO_MODE_INPUT | GPIO_CNF_INPUT_PULL, 1);

	gpio_setup_pin(&gpio_config, sensor_cs_io, GPIO_MODE_OUTPUT_10MHZ | GPIO_CNF_OUTPUT_GENERAL_PUSH_PULL, 1);
	gpio_setup_pin(&gpio_config, sensor_sck_io, GPIO_MODE_OUTPUT_50MHZ | GPIO_CNF_OUTPUT_ALTERNATE_PUSH_PULL, 0);
	gpio_setup_pin(&gpio_config, sensor_miso_io, GPIO_MODE_INPUT | GPIO_CNF_INPUT_FLOATING, 0);
	gpio_setup_pin(&gpio_config, sensor_mosi_io, GPIO_MODE_OUTPUT_50MHZ | GPIO_CNF_OUTPUT_ALTERNATE_PUSH_PULL, 0);
#endif

	gpio_apply_config(gpio_config);

#if defined(SENSOR_ENABLED) && SENSOR_DRIVER == PIXART_PMW
	spi_init_interface(SENSOR_INTERFACE, SPI_MODE3, SENSOR_INTERFACE_SPEED, SPI_MSB_FIRST);

	struct spi_device_t sensor_spi_device = spi_init_device(SENSOR_INTERFACE, sensor_cs_io, SENSOR_INTERFACE_CS_POL);
	struct spi_hal_t sensor_spi_hal = spi_hal_init(&sensor_spi_device);

	struct ticks_hal_t ticks_hal = ticks_hal_init();

	struct pixart_pmw_driver_t sensor = pixart_pmw_init((u8 *) SENSOR_FIRMWARE_BLOB, sensor_spi_hal, ticks_hal);
#endif

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

	usb_init();

	struct output_report report;
	u8 new_data = 0;

	for (;;) {
		tud_task();

#if defined(SENSOR_ENABLED) && SENSOR_DRIVER == PIXART_PMW
		if (!gpio_get(sensor_motion_io)) {
			pixart_pmw_motion_event(&sensor);
		}

		if (sensor.motion_flag) {
			pixart_pmw_read_motion(&sensor);

			new_data = 1;
		}

		if (tud_hid_n_ready(0) && new_data) {
			struct deltas_t deltas = pixart_pmw_get_deltas(&sensor);

			/* fill report */
			memset(&report, 0, sizeof(report));
			report.id = OI_MOUSE_REPORT_ID;
			report.x = deltas.dx;
			report.y = deltas.dy;

			tud_hid_n_report(1, 0, &report, sizeof(report));

			new_data = 0;
		}
#endif
	}
}
