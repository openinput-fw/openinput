/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

enum timer_no {
	timer1,
	timer2,
};

enum cc_no {
	cc1,
	cc2,
	cc3,
	cc4,
};

struct led_pwm_data_t {
	u32 *red_reg;
	u32 *green_reg;
	u32 *blue_reg;
};

struct led_pwm_data_t
led_pwm_init(enum timer_no timer_no, enum cc_no red_ch, enum cc_no green_ch, enum cc_no blue_ch, u16 frequency);
void led_pwm_set_color(struct led_pwm_data_t led_data, u8 red, u8 green, u8 blue);
