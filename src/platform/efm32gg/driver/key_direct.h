/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

#include "platform/efm32gg/gpio.h"

#include "key_actions/key_actions.h"

struct key_direct_t {
	struct gpio_pin_t pin;
	u32 change_timestamp;
	u8 active_low : 1;
	struct key_t key;
};

struct key_direct_t key_direct_init(struct gpio_pin_t pin, u8 active_low);
void key_direct_update(struct key_direct_t *key);
