/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#include "platform/avr/hal/hid.h"
#include "platform/avr/usb.h"

int hid_hal_send(struct hid_hal_t interface, u8 *buffer, size_t buffer_size)
{
	usb_write_descriptor(0, buffer, buffer_size);
}

struct hid_hal_t hid_hal_init(void)
{
	struct hid_hal_t hal = {
		.send = hid_hal_send,
		.drv_data = NULL,
	};
	return hal;
}
