// SPDX-License-Identifier: MIT

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
#include "platform/linux-uhid/uhid.h"
#include "protocol/protocol.h"
#include "protocol/reports.h"

#define OI_MOUSE_REPORT_ID 0x01

static const u8 rdesc[] = {
	/* clang-format off */
	0x05, 0x01,	/* USAGE_PAGE (Generic Desktop) */
	0x09, 0x02,	/* USAGE (Mouse) */
	0xa1, 0x01,	/* COLLECTION (Application) */
	0x09, 0x01,		/* USAGE (Pointer) */
	0xa1, 0x00,		/* COLLECTION (Physical) */
	0x85, 0x01,			/* REPORT_ID (0x01) */
	0x05, 0x01,			/* USAGE_PAGE (Generic Desktop) */
	0x09, 0x30,			/* USAGE (X) */
	0x09, 0x31,			/* USAGE (Y) */
	0x09, 0x38,			/* USAGE (WHEEL) */
	0x15, 0x81,			/* LOGICAL_MINIMUM (-127) */
	0x25, 0x7f,			/* LOGICAL_MAXIMUM (127) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (3) */
	0x81, 0x06,			/* INPUT (Data,Var,Rel) */
	0x05, 0x09,			/* USAGE_PAGE (Button) */
	0x19, 0x01,			/* USAGE_MINIMUM (Button 1) */
	0x29, 0x03,			/* USAGE_MAXIMUM (Button 3) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x01,			/* LOGICAL_MAXIMUM (1) */
	0x95, 0x03,			/* REPORT_COUNT (3) */
	0x75, 0x01,			/* REPORT_SIZE (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0x95, 0x01,			/* REPORT_COUNT (1) */
	0x75, 0x05,			/* REPORT_SIZE (5) */
	0x81, 0x01,			/* INPUT (Cnst,Var,Abs) */
	0xc0,			/* END_COLLECTION */
	0xc0,		/* END_COLLECTION */
	/* clang-format on */
};

struct output_report {
	u8 id;
	s8 x;
	s8 y;
	s8 wheel;
	u8 button1 : 1;
	u8 button2 : 1;
	u8 button3 : 1;
} __attribute__((__packed__));

static char *usage = "commands:\n"
		     "\thelp               \t\tshows this help message\n"
		     "\tmove <axis> <value>\t\tmoves an axis\n"
		     "\texit               \t\texit the program\n";

struct uhid_dispatch_args_t {
	struct hid_interface_t uhid;
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
	struct hid_interface_t uhid;
	struct output_report report;
	struct uhid_create2_req create;

	pthread_t uhid_dispatch_thread;
	struct uhid_dispatch_args_t args;
	int uhid_dispatch_exit = 0;
	unsigned int rdesc_size = 0;

	struct supported_functions_t functions = {
		/* clang-format off */
		.info = (u8[]) {
			OI_FUNCTION_VERSION,
			OI_FUNCTION_FW_INFO
		}
		/* clang-format on */
	};
	struct protocol_config_t config = {"openinput Linux UHID Device", functions, &uhid};

	char *line = NULL;
	char *arg = NULL;
	int ret = 0;

	char *endptr = NULL;

	char axis;
	int value;

	/* open uhid fd */
	ret = uhid_open(&uhid);
	if (ret)
		goto exit;

	/* create uhid device */
	memset(&create, 0, sizeof(create));
	strcpy(create.name, config.device_name);
	memcpy(create.rd_data, oi_rdesc, sizeof(oi_rdesc)); /* protocol report descriptor */
	rdesc_size += sizeof(oi_rdesc);
	memcpy(create.rd_data + rdesc_size, rdesc, sizeof(rdesc)); /* mouse report descriptor */
	rdesc_size += sizeof(rdesc);
	create.rd_size = rdesc_size;
	create.bus = 0x03;
	create.vendor = 0x9999;
	create.product = 0x9999;
	ret = uhid_create(uhid, create);
	if (ret)
		goto exit;

	/* wait for the kernel to respond to the uhid creation request */
	uhid_wait_for_kernel_start(uhid);

	/* start uhid dispatch thread */
	args.uhid = uhid;
	args.config = config;
	args.exit = &uhid_dispatch_exit;
	pthread_create(&uhid_dispatch_thread, NULL, uhid_dispatch, &args);

	/* main loop */
	for (;;) {
		/* process shell - user action */
		line = readline("# ");
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
			report.id = OI_MOUSE_REPORT_ID;
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
	pthread_join(uhid_dispatch_thread, NULL);

	free(line);
	close(uhid.epoll_fd);
	close(uhid.uhid_fd);

	return ret;
}
