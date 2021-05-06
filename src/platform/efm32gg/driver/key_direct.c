/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "platform/efm32gg/driver/key_direct.h"
#include "platform/efm32gg/systick.h"

struct key_direct_t key_direct_init(struct gpio_pin_t pin, u8 active_low)
{
	struct key_direct_t key;

	key.pin = pin;
	key.active_low = active_low;
	key.key.key_status = key_released;
	key.change_timestamp = systick_get_ticks();

	return key;
}

void key_direct_update(struct key_direct_t *key)
{
	enum key_status_t last_status = key->key.key_status;

	key->key.key_status = (gpio_get(key->pin) ^ key->active_low) ? key_pressed : key_released;

	if (key->key.key_status != last_status)
		key->change_timestamp = systick_get_ticks();

	key->key.key_t_delta = systick_get_ticks() - key->change_timestamp;
}
