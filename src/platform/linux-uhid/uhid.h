// SPDX-License-Identifier: MIT

#include <linux/uhid.h>
#include <sys/epoll.h>

#define MAX_EPOLL_EVENTS 10

struct hid_interface_t {
	int uhid_fd;
	struct epoll_event epoll_events[MAX_EPOLL_EVENTS];
};

int uhid_open(struct hid_interface_t *interface);

int uhid_create(struct hid_interface_t interface, struct uhid_create2_req request);

int uhid_read_event(struct hid_interface_t interface, struct uhid_event *event);

int uhid_send(struct hid_interface_t interface, u8 *buffer, size_t buffer_len);
