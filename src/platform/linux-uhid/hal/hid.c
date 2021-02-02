// SPDX-License-Identifier: MIT

#include "hal/hid.h"
#include "platform/linux-uhid/uhid.h"
#include "util/types.h"

int hid_send(struct hid_interface_t interface, u8 *buffer, size_t buffer_size)
{
	return uhid_send(interface, buffer, buffer_size);
}
