/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "util/data.h"
#include "util/types.h"

#include "platform/samx7x/eefc.h"
#include "platform/samx7x/hal/blockdev.h"
#include "platform/samx7x/hal/hid.h"
#include "platform/samx7x/hal/spi.h"
#include "platform/samx7x/hal/ticks.h"
#include "platform/samx7x/pio.h"
#include "platform/samx7x/pmc.h"
#include "platform/samx7x/qspi.h"
#include "platform/samx7x/systick.h"
#include "platform/samx7x/usb.h"
#include "platform/samx7x/wdt.h"

#include "driver/pixart/pixart_pmw.h"

#include "util/hid_descriptors.h"
#include "util/partition/partition.h"

#include "protocol/protocol.h"

#define CFG_TUSB_CONFIG_FILE "targets/sams70-generic/tusb_config.h"
#include "tusb.h"

extern u32 _stable;

void main()
{
	eefc_init();
	eefc_tcm_disable();

	pmc_init(EXTERNAL_CLOCK_VALUE, 0UL);
	pmc_init_usb();
	pmc_update_clock_tree();

	systick_init();

	wdt_disable();

	pio_init();

	const struct partition_table_t *partition_table = partition_table_read((void *) &_stable);

	const struct partition_table_entry_t *sensor_blob = partition_from_type(partition_table, PARTITION_TYPE_BLOB);

	const struct partition_table_entry_t *nvs_data = partition_from_type(partition_table, PARTITION_TYPE_NVS);

	// static struct blockdev_drv_t nvs_block_drv;
	// nvs_block_drv.start_addr = nvs_data->start_addr;
	// nvs_block_drv.size = (nvs_data->end_addr - nvs_data->start_addr);
	// struct blockdev_hal_t data_blockdev_hal = blockdev_hal_init_eefc(&nvs_block_drv);

	// data_blockdev_hal.erase(data_blockdev_hal, 0);
	// u8 buffer[512];
	// memset(buffer, 0x5A, 512);
	// data_blockdev_hal.write(data_blockdev_hal, 0, 16, buffer, 496);

	/* could not find sensor blob, halt */
	if (!sensor_blob)
		while (1) continue;

#if defined(SENSOR_ENABLED) && SENSOR_DRIVER == PIXART_PMW
	struct pio_pin_t sensor_motion_io = SENSOR_MOTION_IO;
	struct pio_pin_t sensor_cs_io = SENSOR_INTERFACE_CS_IO;
	struct pio_pin_t sensor_sck_io = SENSOR_INTERFACE_SCK_IO;
	struct pio_pin_t sensor_miso_io = SENSOR_INTERFACE_MISO_IO;
	struct pio_pin_t sensor_mosi_io = SENSOR_INTERFACE_MOSI_IO;

	pio_config(sensor_motion_io, PIO_DIRECTION_IN, 0, PIO_PULL_UP, PIO_MUX_A, 0);

	pio_config(sensor_cs_io, PIO_DIRECTION_OUT, 1, PIO_PULL_NONE, PIO_MUX_A, 0);

	pio_config(sensor_sck_io, PIO_DIRECTION_OUT, 1, PIO_PULL_NONE, PIO_MUX_A, PIO_PERIPHERAL_CTRL);
	pio_config(sensor_miso_io, PIO_DIRECTION_IN, 0, PIO_PULL_UP, PIO_MUX_A, PIO_PERIPHERAL_CTRL);
	pio_config(sensor_mosi_io, PIO_DIRECTION_OUT, 1, PIO_PULL_NONE, PIO_MUX_A, PIO_PERIPHERAL_CTRL);

	qspi_init_interface(QSPI_MODE3, SENSOR_INTERFACE_SPEED);

	struct qspi_device_t sensor_spi_device = qspi_init_device(sensor_cs_io, SENSOR_INTERFACE_CS_POL);
	struct spi_hal_t sensor_spi_hal = spi_hal_init_qspi(&sensor_spi_device);

	struct ticks_hal_t ticks_hal = ticks_hal_init();

	struct pixart_pmw_driver_t sensor = pixart_pmw_init((const u8 *) sensor_blob->start_addr, sensor_spi_hal, ticks_hal);
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

	struct mouse_report report;
	u8 new_data = 0;

	for (;;) {
		tud_task();

#if defined(SENSOR_ENABLED) && SENSOR_DRIVER == PIXART_PMW
		if (!pio_get(sensor_motion_io)) {
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
			report.id = MOUSE_REPORT_ID;
			report.x = deltas.dx;
			report.y = deltas.dy;

			tud_hid_n_report(1, 0, &report, sizeof(report));

			new_data = 0;
		}
#endif
	}
}
