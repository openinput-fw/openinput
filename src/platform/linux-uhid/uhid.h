/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

#include <linux/uhid.h>
#include <sys/epoll.h>

#define MAX_EPOLL_EVENTS 10

struct uhid_data_t {
	int uhid_fd;
	int epoll_fd;
	struct epoll_event epoll_events[MAX_EPOLL_EVENTS];
};

int uhid_open(struct uhid_data_t *data);
void uhid_close(struct uhid_data_t data);
int uhid_create(struct uhid_data_t data, struct uhid_create2_req request);
int uhid_read_event(struct uhid_data_t data, struct uhid_event *event);
void uhid_wait_for_kernel_start(struct uhid_data_t data);
int uhid_wait_for_events(struct uhid_data_t data, int timeout);
int uhid_send(struct uhid_data_t data, u8 *buffer, size_t buffer_len);
