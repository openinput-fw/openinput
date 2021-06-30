/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

struct ticks_hal_t {
	/* wait for x milisecs */
	void (*delay_ms)(u32 ticks);
	/* wait for x microsecs */
	void (*delay_us)(u32 ticks);
	/* arbitrary user data */
	void *drv_data;
};
