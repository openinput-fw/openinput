/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hal/hid.h"
#include "uhid.h"
#include "util/types.h"

static int uhid_write(struct uhid_data_t data, struct uhid_event event)
{
	size_t ret = write(data.uhid_fd, &event, sizeof(event));
	if (ret < 0) {
		fprintf(stderr, "error: couldn't write to uhid (%m)\n");
		return -errno;
	} else if (ret != sizeof(event)) {
		fprintf(stderr, "error: failed to write to uhid (tried to write %d, wrote %d)", sizeof(event), ret);
		return -EFAULT;
	}
	return 0;
}

int uhid_open(struct uhid_data_t *data)
{
	struct epoll_event epoll_event;

	/* open uhid fd */
	data->uhid_fd = open("/dev/uhid", O_RDWR | O_CLOEXEC);
	if (data->uhid_fd == -1) {
		fprintf(stderr, "error: cannot open uhid (%m)\n");
		return -errno;
	}

	/* create epoll */
	data->epoll_fd = epoll_create1(0);
	if (data->epoll_fd == -1) {
		fprintf(stderr, "error: failed to open epoll fd (%m)\n");
		return -errno;
	}

	/* register uhid fd in epoll */
	epoll_event.events = EPOLLIN;
	epoll_event.data.fd = data->uhid_fd;
	if (epoll_ctl(data->epoll_fd, EPOLL_CTL_ADD, data->uhid_fd, &epoll_event)) {
		fprintf(stderr, "error: failed to register uhid fd in epoll (%m)\n");
		return -errno;
	}

	return 0;
}

void uhid_close(struct uhid_data_t data)
{
	close(data.uhid_fd);
	close(data.epoll_fd);
}

int uhid_create(struct uhid_data_t data, struct uhid_create2_req request)
{
	struct uhid_event event = {UHID_CREATE2, .u.create2 = request};
	int ret = uhid_write(data, event);
	if (ret)
		fprintf(stderr, "failed to create uhid device\n");
	return ret;
}

int uhid_read_event(struct uhid_data_t data, struct uhid_event *event)
{
	int ret;

	memset(event, 0, sizeof(*event));
	ret = read(data.uhid_fd, event, sizeof(*event));
	if (ret == -1) {
		fprintf(stderr, "error: failed to read uhid event from epoll (%m)\n");
		return -errno;
	} else if (ret != sizeof(*event)) {
		fprintf(stderr,
			"error: failed to read uhid event from epoll (read %d but was expecting %d)\n",
			ret,
			sizeof(*event));
		return -EFAULT;
	}
	return 0;
}

void uhid_wait_for_kernel_start(struct uhid_data_t data)
{
	int event_count;
	struct uhid_event event;
	for (;;) {
		event_count = epoll_wait(data.epoll_fd, data.epoll_events, 1, 3000);
		uhid_read_event(data, &event);
		if (event.type == UHID_START)
			break;
	}
}

int uhid_wait_for_events(struct uhid_data_t data, int timeout)
{
	return epoll_wait(data.epoll_fd, data.epoll_events, MAX_EPOLL_EVENTS, timeout);
}

int uhid_send(struct uhid_data_t data, u8 *buffer, size_t buffer_len)
{
	int ret;
	struct uhid_event event = {
		.type = UHID_INPUT2,
		.u.input2.size = buffer_len,
	};
	memcpy(event.u.input2.data, buffer, buffer_len);

	if (buffer_len > sizeof(event.u.input2.data)) {
		fprintf(stderr, "error: packet too big (size is %d, max is %d)\n", buffer_len, sizeof(event.u.input2.data));
		return -EINVAL;
	}

	ret = uhid_write(data, event);
	if (ret)
		fprintf(stderr, "failed to send report to device\n");
	return ret;
}
