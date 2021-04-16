/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#include "platform/efm32gg/hal/hid.h"

#define CFG_TUSB_CONFIG_FILE "targets/efm32gg12-generic/tusb_config.h"
#include "tusb.h"

int hid_hal_send(struct hid_hal_t interface, u8 *buffer, size_t buffer_size)
{
	tud_hid_report(0, buffer, buffer_size);
}

struct hid_hal_t hid_hal_init(void)
{
	struct hid_hal_t hal = {
		.send = hid_hal_send,
		.drv_data = NULL,
	};
	return hal;
}
