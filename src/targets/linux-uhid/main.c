/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "readline/history.h"
#include "readline/readline.h"

#include "hal/hid.h"
#include "platform/linux-uhid/hal/hid.h"
#include "platform/linux-uhid/uhid.h"
#include "protocol/protocol.h"
#include "protocol/reports.h"
#include "util/hid_descriptors.h"
#include "util/usb_descriptors.h"

static char *usage = "commands:\n"
		     "\thelp               \t\tshows this help message\n"
		     "\tmove <axis> <value>\t\tmoves an axis\n"
		     "\texit               \t\texit the program\n";

struct uhid_dispatch_args_t {
	struct uhid_data_t uhid;
	struct protocol_config_t config;
	int *exit;
};

void *uhid_dispatch(void *thread_args)
{
	struct uhid_dispatch_args_t *args = (struct uhid_dispatch_args_t *) thread_args;
	struct uhid_event event;
	size_t event_count;

	while (!*args->exit) {
		/* handle incoming packets */
		event_count = uhid_wait_for_events(args->uhid, 100);

		for (size_t i = 0; i < event_count; i++) {
			uhid_read_event(args->uhid, &event);
			switch (event.type) {
				case UHID_OUTPUT:
					protocol_dispatch(args->config, event.u.output.data, event.u.output.size);
					break;
				case UHID_GET_REPORT:
					/* TODO */
					break;
				case UHID_SET_REPORT:
					/* TODO */
					break;
				case UHID_OPEN:
				case UHID_CLOSE:
					/* ignore */
					break;
				default:
					break;
			}
		}
	}
}

int main(void)
{
	struct uhid_data_t uhid;
	struct mouse_report report;
	struct uhid_create2_req create;

	pthread_t uhid_dispatch_thread = 0;
	struct uhid_dispatch_args_t args;
	int uhid_dispatch_exit = 0;
	unsigned int rdesc_size = 0;

	struct hid_hal_t hid_hal;
	struct protocol_config_t config;
	u8 info_functions[] = {
		OI_FUNCTION_VERSION,
		OI_FUNCTION_FW_INFO,
		OI_FUNCTION_SUPPORTED_FUNCTION_PAGES,
		OI_FUNCTION_SUPPORTED_FUNCTIONS,
	};

	char *line = NULL;
	char *arg = NULL;
	int ret = 0;

	char *endptr = NULL;

	char axis;
	int value;

	/* create protocol config */
	memset(&config, 0, sizeof(config));
	config.device_name = "openinput Linux UHID Device";
	config.hid_hal = uhid_hid_hal_init(&uhid);
	config.functions[INFO] = info_functions;
	config.functions_size[INFO] = sizeof(info_functions);

	/* open uhid fd */
	ret = uhid_open(&uhid);
	if (ret)
		goto exit;

	/* create uhid device */
	memset(&create, 0, sizeof(create));
	strcpy(create.name, config.device_name);
	memcpy(create.rd_data, oi_rdesc, sizeof(oi_rdesc)); /* protocol report descriptor */
	rdesc_size += sizeof(oi_rdesc);
	memcpy(create.rd_data + rdesc_size,
	       desc_hid_mouse_report,
	       sizeof(desc_hid_mouse_report)); /* mouse report descriptor */
	rdesc_size += sizeof(desc_hid_mouse_report);
	create.rd_size = rdesc_size;
	create.bus = BUS_USB;
	create.vendor = VENDOR_ID;
	create.product = PRODUCT_ID;
	create.version = RELEASE;
	ret = uhid_create(uhid, create);
	if (ret)
		goto exit;

	/* wait for the kernel to respond to the uhid creation request */
	uhid_wait_for_kernel_start(uhid);

	/* start uhid dispatch thread */
	args.uhid = uhid;
	args.config = config;
	args.exit = &uhid_dispatch_exit;
	if (pthread_create(&uhid_dispatch_thread, NULL, uhid_dispatch, &args)) {
		uhid_dispatch_thread = 0;
		goto exit;
	}

	/* main loop */
	for (;;) {
		/* process shell - user action */
		line = readline("# ");
		if (!line)
			goto exit;

		add_history(line);
		arg = strtok(line, " ");

		if (!arg)
			continue;

		/* TODO: implement history support using GNU history */
		if (strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0) {
			printf(usage);
		} else if (strcmp(arg, "move") == 0) {
			/* get axis argument */
			arg = strtok(NULL, " ");
			if (!arg) {
				fprintf(stderr, "error: missing axis argument! (usage: move <axis> <value>)\n");
				ret = -EINVAL;
				goto exit;
			}

			/* validate axis */
			if (strcmp(arg, "x") == 0 || strcmp(arg, "X") == 0)
				axis = 'X';
			else if (strcmp(arg, "y") == 0 || strcmp(arg, "Y") == 0)
				axis = 'Y';
			else {
				fprintf(stderr, "error: unknown axis (%s)\n", arg);
				ret = -EINVAL;
				goto exit;
			}

			/* get value argument */
			arg = strtok(NULL, " ");
			if (!arg) {
				fprintf(stderr, "error: missing value argument! (usage: move <axis> <value>)\n");
				ret = -EINVAL;
				goto exit;
			}

			/* parse value */
			value = strtol(arg, &endptr, 0);
			if (errno || *endptr) {
				fprintf(stderr, "error: invalid value");
				ret = -EINVAL;
				goto exit;
			} else if (value > 127) {
				fprintf(stderr, "error: value too big (max is 127)\n");
				ret = -EINVAL;
				goto exit;
			} else if (value < -127) {
				fprintf(stderr, "error: value too small (min is -127)\n");
				ret = -EINVAL;
				goto exit;
			}

			printf("moving %d in %c axis\n", value, axis);

			/* fill report */
			memset(&report, 0, sizeof(report));
			report.id = MOUSE_REPORT_ID;
			if (axis == 'X')
				report.x = value;
			else if (axis == 'Y')
				report.y = value;

			/* send */
			uhid_send(uhid, (u8 *) &report, sizeof(report));
		} else if (strcmp(arg, "exit") == 0) {
			ret = 0;
			goto exit;
		} else {
			fprintf(stderr, "error: unkown command\n");
			ret = -EINVAL;
			goto exit;
		}
		free(line);
	}

exit:
	uhid_dispatch_exit = 1;
	if (uhid_dispatch_thread)
		pthread_join(uhid_dispatch_thread, NULL);

	free(line);
	uhid_close(uhid);

	return ret;
}
