/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <errno.h>
#include <string.h>

#include "protocol/protocol.h"
#include "protocol/reports.h"

#include "hal/hid.h"
#include "util/data.h"
#include "util/types.h"

#include <stdio.h>

void protocol_get_functions_pages(struct protocol_config_t config, u8 *pages, u8 *pages_size)
{
	u8 index = 0;

	for (u8 i = 0; i < PAGE_COUNT; i++)
		if (config.functions[i] != NULL && config.functions_size[i] != 0)
			pages[index++] = supported_pages[i];
		else
			--(*pages_size);
}

u8 *protocol_get_functions(struct protocol_config_t config, u8 function_page, size_t *functions_size)
{
	for (size_t i = 0; i < sizeof(supported_pages); i++) {
		if (supported_pages[i] == function_page) {
			*functions_size = config.functions_size[i];
			return config.functions[i];
		}
	}
	return NULL;
}

u8 protocol_get_smallest_report_id(size_t args_size)
{
	if (args_size <= OI_REPORT_SHORT_DATA_MAX_SIZE - 2)
		return OI_REPORT_SHORT;
	return OI_REPORT_LONG;
}

int protocol_is_supported(struct protocol_config_t config, u8 function_page, u8 function)
{
	size_t functions_size;
	u8 *functions = protocol_get_functions(config, function_page, &functions_size);

	for (size_t i = 0; i < functions_size; i++)
		if (functions[i] == function)
			return 1;
	return 0;
}

void protocol_dispatch(struct protocol_config_t config, u8 *buffer, size_t buffer_size)
{
	struct oi_report_t msg;
	struct protocol_error_t unsupported_error = {
		.id = OI_ERROR_UNSUPPORTED_FUNCTION,
	};
	int ret;

	if (buffer_size < 1)
		/* needs at least a report ID */
		return;

	memcpy(&msg, buffer, buffer_size);

	switch (msg.id) {
		case OI_REPORT_SHORT:
			if (buffer_size != OI_REPORT_SHORT_SIZE)
				return;
			break;

		case OI_REPORT_LONG:
			if (buffer_size != OI_REPORT_LONG_SIZE)
				return;
			break;

		default:
			return;
	}

	if (!protocol_is_supported(config, msg.function_page, msg.function)) {
		protocol_send_error(config, msg, unsupported_error);
		return;
	}

	switch (msg.function_page) {
		case OI_PAGE_INFO:
			switch (msg.function) {
				case OI_FUNCTION_VERSION:
					protocol_info_version(config, msg);
					break;
				case OI_FUNCTION_FW_INFO:
					protocol_info_fw_info(config, msg);
					break;
				case OI_FUNCTION_SUPPORTED_FUNCTION_PAGES:
					protocol_info_supported_function_pages(config, msg);
					break;
				case OI_FUNCTION_SUPPORTED_FUNCTIONS:
					protocol_info_supported_functions(config, msg);
					break;
				default:
					break;
			}

		default:
			break;
	}
}

void protocol_send_report(struct protocol_config_t config, struct oi_report_t msg)
{
	int ret = 0;

	switch (msg.id) {
		case OI_REPORT_SHORT:
			ret = config.hid_hal.send(config.hid_hal, (u8 *) &msg, OI_REPORT_SHORT_SIZE);
			break;
		case OI_REPORT_LONG:
			ret = config.hid_hal.send(config.hid_hal, (u8 *) &msg, OI_REPORT_LONG_SIZE);
			break;
		default:
			break;
	}
}

void protocol_send_error(struct protocol_config_t config, struct oi_report_t msg, struct protocol_error_t error)
{
	msg.data[0] = msg.function_page;
	msg.data[1] = msg.function;
	memcpy(msg.data + 2, &error.args, sizeof(msg.data - 2));
	msg.function_page = OI_PAGE_ERROR;
	msg.function = error.id;

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

void protocol_info_fw_info(struct protocol_config_t config, struct oi_report_t msg)
{
	struct protocol_error_t error = {.id = OI_ERROR_INVALID_VALUE};

	msg.id = OI_REPORT_LONG;
	switch (msg.data[0]) {
		case 0: /* fw vendor */
			snprintf(msg.data, sizeof(msg.data), "%s", OI_VENDOR);
			break;
		case 1: /* fw version */
			snprintf(msg.data, sizeof(msg.data), "%s", OI_VERSION);
			break;
		case 2: /* device name */
			snprintf(msg.data, sizeof(msg.data), "%s", config.device_name);
			break;
		default:
			error.args.invalid_value.position = 0;
			protocol_send_error(config, msg, error);
			return;
	}

	protocol_send_report(config, msg);
}

void protocol_info_supported_function_pages(struct protocol_config_t config, struct oi_report_t msg)
{
	struct protocol_error_t error = {
		.id = OI_ERROR_INVALID_VALUE,
	};
	u8 copy_size;
	u8 pages[PAGE_COUNT];
	u8 pages_size = sizeof(pages);
	u8 start_index = msg.data[0];

	protocol_get_functions_pages(config, pages, &pages_size);

	if (start_index >= pages_size) {
		error.args.invalid_value.position = 0;
		protocol_send_error(config, msg, error);
		return;
	}

	copy_size = min(sizeof(msg.data) - 1, pages_size - start_index);
	msg.id = protocol_get_smallest_report_id(copy_size);
	msg.data[0] = copy_size;
	msg.data[1] = pages_size - start_index - copy_size;
	memcpy(msg.data + 2, pages + start_index, copy_size);

	protocol_send_report(config, msg);
}

void protocol_info_supported_functions(struct protocol_config_t config, struct oi_report_t msg)
{
	struct protocol_error_t error = {
		.id = OI_ERROR_INVALID_VALUE,
	};
	u8 copy_size;
	size_t functions_size;
	u8 *functions = protocol_get_functions(config, msg.data[0], &functions_size);
	u8 start_index = msg.data[1];

	if (start_index >= functions_size) {
		error.args.invalid_value.position = 1;
		protocol_send_error(config, msg, error);
		return;
	}

	copy_size = min(sizeof(msg.data) - 1, functions_size - start_index);
	msg.id = protocol_get_smallest_report_id(copy_size);
	msg.data[0] = copy_size;
	msg.data[1] = functions_size - start_index - copy_size;
	memcpy(msg.data + 2, functions + start_index, copy_size);

	protocol_send_report(config, msg);
}
