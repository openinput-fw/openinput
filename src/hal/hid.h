/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include "util/types.h"

struct hid_interface_t;

int hid_send(void *interface, u8 *buffer, size_t buffer_size);
