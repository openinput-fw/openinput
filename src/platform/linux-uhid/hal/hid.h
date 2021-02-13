/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

#include "hal/hid.h"
#include "platform/linux-uhid/uhid.h"
#include "util/types.h"

struct hid_hal_t uhid_hal_init(struct uhid_data_t *data);
int uhid_hal_hid_send(struct hid_hal_t interface, u8 *buffer, size_t buffer_size);
