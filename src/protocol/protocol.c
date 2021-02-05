// SPDX-License-Identifier: MIT

#include <errno.h>
#include <string.h>

#include "protocol/protocol.h"
#include "protocol/reports.h"

#include "hal/hid.h"
#include "util/types.h"

#include <stdio.h>

int protocol_is_supported(struct protocol_config_t config, u16 function)
{
	for (size_t i = 0; i < config.supported_functions_lenght; i++)
		if (config.supported_functions[i] == function)
			return 1;
	return 0;
}

void protocol_dispatch(struct protocol_config_t config, u8 *buffer, size_t buffer_size)
{
	struct oi_report_t msg;
	int ret;

	if (buffer_size < 1)
		/* needs at least a report ID */
		return;

	memcpy(&msg, buffer, buffer_size);

	if (msg.id == OI_REPORT_SHORT && buffer_size != OI_REPORT_SHORT_SIZE)
		return;
	if (msg.id == OI_REPORT_LONG && buffer_size != OI_REPORT_LONG_SIZE)
		return;

	if (!protocol_is_supported(config, msg.function))
		/* TODO: send error*/
		return;

	switch (msg.function) {
		case FN(OI_PAGE_INFO, OI_FUNCTION_VERSION):
			protocol_info_version(config, msg);
			break;

		default:
			break;
	}
}

void protocol_send_report(struct protocol_config_t config, struct oi_report_t msg)
{
	int ret = 0;

	switch (msg.id) {
		case OI_REPORT_SHORT:
			ret = hid_send(config.hid_interface, (u8 *) &msg, OI_REPORT_SHORT_SIZE);
			break;
		case OI_REPORT_LONG:
			ret = hid_send(config.hid_interface, (u8 *) &msg, OI_REPORT_LONG_SIZE);
			break;
		default:
			break;
	}
}

void protocol_send_error(struct protocol_config_t config, struct oi_report_t msg, struct protocol_error_t error)
{
	msg.data[0] = (msg.function & 0xFF00) >> 8;
	msg.data[1] = msg.function & 0x00FF;
	memcpy(msg.data + 2, &error.args, sizeof(msg.data - 2));
	msg.function = FN(OI_PAGE_ERROR, error.id);

	protocol_send_report(config, msg);
}

/*
 * 0x00 - info
 */

void protocol_info_version(struct protocol_config_t config, struct oi_report_t msg)
{
	msg.id = OI_REPORT_SHORT;
	msg.data[0] = OI_PROTOCOL_VERSION_MAJOR;
	msg.data[1] = OI_PROTOCOL_VERSION_MINOR;
	msg.data[2] = OI_PROTOCOL_VERSION_PATCH;

	protocol_send_report(config, msg);
}
