/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "platform/efm32gg/driver/key_matrix.h"
#include "platform/efm32gg/systick.h"

struct key_matrix_t key_matrix_init(
	struct gpio_pin_t *col_pin_list,
	u8 col_pin_list_size,
	struct gpio_pin_t *row_pin_list,
	u8 row_pin_list_size,
	struct key_t *key_data,
	u8 col_to_row)
{
	struct key_matrix_t matrix = {};

	matrix.col_pin_list = col_pin_list;
	matrix.col_pin_list_size = col_pin_list_size;
	matrix.row_pin_list = row_pin_list;
	matrix.row_pin_list_size = row_pin_list_size;
	matrix.key_data = key_data;
	matrix.col_to_row = col_to_row;
	matrix.update_timestamp = systick_get_ticks();

	for (size_t col = 0; col < matrix.col_pin_list_size; col++) gpio_set(col_pin_list[col], !matrix.col_to_row);

	return matrix;
}

void key_matrix_scan(struct key_matrix_t *matrix)
{
	u16 t_delta = systick_get_ticks() - matrix->update_timestamp;
	matrix->update_timestamp = systick_get_ticks();

	for (size_t col = 0; col < matrix->col_pin_list_size; col++) {
		/* set col output */
		gpio_set(matrix->col_pin_list[col], matrix->col_to_row);

		for (size_t row = 0; row < matrix->row_pin_list_size; row++) {
			u8 index = (col * matrix->row_pin_list_size) + row;
			enum key_status_t last_status = matrix->key_data[index].key_status;

			/* get row input */
			matrix->key_data[index].key_status =
				(gpio_get(matrix->row_pin_list[row]) ^ matrix->col_to_row) ? key_released : key_pressed;

			if (matrix->key_data[index].key_status != last_status)
				matrix->key_data[index].key_t_delta = 0;
			else
				matrix->key_data[index].key_t_delta += t_delta;
		}

		/* clear col output */
		gpio_set(matrix->col_pin_list[col], !matrix->col_to_row);

		/* settle time */
		delay_us(1);
	}
}
