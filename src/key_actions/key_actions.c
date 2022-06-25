/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "key_actions/key_actions.h"
#include "util/data.h"

struct key_actions_return_t key_actions_update(struct key_actions_config_t config)
{
	u8 keycode_index = 0;
	struct key_actions_return_t return_data = {};

	for (size_t i = 0; i < config.key_list_size; i++) {
		switch (config.active_layer[i].node_action) {
			case layer_node_none:
				break;

			case layer_node_regular:
				if (config.key_list[i]->key_status == key_pressed) {
					if (keycode_index < 6)
						return_data.keycodes[keycode_index++] = config.active_layer[i].data[0];
					return_data.data_present = 1;
				}
				break;

			case layer_node_tap_hold:
				if (config.key_list[i]->key_status == key_pressed) {
					if (config.key_list[i]->key_t_delta > config.tap_interval) {
						config.active_layer[i].data[3] = 0;
						if (keycode_index < 6)
							return_data.keycodes[keycode_index++] = config.active_layer[i].data[1];
						return_data.data_present = 1;
					} else {
						config.active_layer[i].data[3] = 1;
					}
				} else {
					if (config.active_layer[i].data[3]) {
						config.active_layer[i].data[3] = 0;
						if (keycode_index < 6)
							return_data.keycodes[keycode_index++] = config.active_layer[i].data[0];
						return_data.data_present = 1;
					}
				}

				break;

			case layer_node_set_layer:
				if (config.key_list[i]->key_status == key_pressed) {
					key_actions_load_layer(config, config.active_layer[i].data[0]);
					return_data.data_present = 1;
				}
				break;

			case layer_node_mouse_buttons:
				if (config.key_list[i]->key_status == key_pressed) {
					u8 but_no = config.active_layer[i].data[0];
					if (but_no >= 0 && but_no <= 5)
						return_data.mouse_buttons |= BIT(but_no);
					return_data.data_present = 1;
				}
				break;
		}
	}

	return return_data;
}

void key_actions_load_layer(struct key_actions_config_t config, u8 layer_no)
{
	if (layer_no >= config.layer_list_size)
		return;

	for (size_t i = 0; i < config.key_list_size; i++) config.active_layer[i] = config.layer_list[layer_no][i];
}
