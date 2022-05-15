/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "hal/blockdev.h"
#include "platform/samx7x/eefc.h"
#include "util/types.h"

struct blockdev_drv_t {
	s32 start_addr;
	u32 size;
};

struct blockdev_hal_t blockdev_hal_init_eefc(struct blockdev_drv_t *drv_data);
