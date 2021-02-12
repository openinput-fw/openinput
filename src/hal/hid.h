/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

#include "util/types.h"

struct hid_hal_t {
	int (*send)(struct hid_hal_t interface, u8 *buffer, size_t buffer_size);
	void *drv_data;
};
