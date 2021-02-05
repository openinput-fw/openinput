// SPDX-License-Identifier: MIT

#include "util/types.h"

struct hid_interface_t;

int hid_send(void *interface, u8 *buffer, size_t buffer_size);
