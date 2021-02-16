/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include "hal/hid.h"
#include "platform/linux-uhid/hal/hid.h"
#include "platform/linux-uhid/uhid.h"

struct hid_hal_t uhid_hid_hal_init(struct uhid_data_t *data)
{
	struct hid_hal_t hal = {
		.send = uhid_hal_hid_send,
		.drv_data = data,
	};
	return hal;
}

int uhid_hal_hid_send(struct hid_hal_t interface, u8 *buffer, size_t buffer_size)
{
	struct uhid_data_t *data = interface.drv_data;
	return uhid_send(*data, buffer, buffer_size);
}
