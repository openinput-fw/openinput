/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stddef.h>

#include "hal/hid.h"
#include "protocol/protocol.h"
#include "util/types.h"

int dummy_hal_hid_send(struct hid_hal_t interface, u8 *buffer, size_t buffer_size)
{
	return buffer_size;
}

int LLVMFuzzerTestOneInput(const char *data, size_t size)
{
	u8 info_functions[] = {
		OI_FUNCTION_VERSION,
		OI_FUNCTION_FW_INFO,
		OI_FUNCTION_SUPPORTED_FUNCTION_PAGES,
		OI_FUNCTION_SUPPORTED_FUNCTIONS,
	};
	struct hid_hal_t hid_hal = {
		.send = dummy_hal_hid_send,
	};
	struct protocol_config_t config = {
		.device_name = "openinput fuzz device",
		.hid_hal = hid_hal,
		.functions = info_functions,
		.functions_size = sizeof(info_functions),
	};

	protocol_dispatch(config, (u8 *) data, size);
	return 0;
}
