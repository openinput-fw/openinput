/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

#include "platform/efm32gg/gpio.h"

#include "key_actions/key_actions.h"

struct key_matrix_t {
	struct gpio_pin_t *col_pin_list;
	u8 col_pin_list_size;
	struct gpio_pin_t *row_pin_list;
	u8 row_pin_list_size;
	struct key_t *key_data;
	u32 update_timestamp;
	u8 col_to_row : 1;
};

struct key_matrix_t key_matrix_init(
	struct gpio_pin_t *col_pin_list,
	u8 col_pin_list_size,
	struct gpio_pin_t *row_pin_list,
	u8 row_pin_list_size,
	struct key_t *key_data,
	u8 col_to_row);
void key_matrix_scan(struct key_matrix_t *matrix);
