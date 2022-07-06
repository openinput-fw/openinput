/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

void systick_init();
u64 systick_get_ticks();
void delay_ms(u32 ticks);
void delay_us(u32 ticks);
