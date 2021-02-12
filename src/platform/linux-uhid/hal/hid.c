/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include "hal/hid.h"
#include "platform/linux-uhid/uhid.h"
#include "util/types.h"

int hid_send(void *interface, u8 *buffer, size_t buffer_size)
{
	return uhid_send(*((struct hid_interface_t *) interface), buffer, buffer_size);
}
