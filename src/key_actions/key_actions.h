/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

enum key_status_t {
	key_released = 0b0,
	key_pressed = 0b1,
};

struct key_t {
	u16 key_status : 1;
	u16 key_t_delta : 15; /* time delta since change, used to determine tap and hold behaviour*/
};

enum layer_node_actions_t {
	layer_node_none, /* no action */
	layer_node_regular, /* regular keystroke, send keycode on data[0] */
	layer_node_tap_hold, /* send keycode on data[0] on tap, keycode on data[1] on hold */
	layer_node_set_layer, /* set layer specified on data[0] */
	layer_node_mouse_buttons, /* send mouse button, index on data[0] (1-5) */
};

struct layer_node_t {
	u8 node_action;
	u8 data[3]; /* data use depends on action */
};

struct key_actions_config_t {
	struct key_t **key_list; /* array of pointers to key_t structs, these are provided by low level platform drivers */
	u8 key_list_size; /* number of key_t elements */
	struct layer_node_t **layer_list; /* array of layers (arrays of layer_node_t, size must be the same as key_list_size) */
	u8 layer_list_size; /* number of available layers on layer_list */
	struct layer_node_t *active_layer; /* used internally, array of layer_node_t, size must be the same as key_list_size */
	u16 tap_interval; /* setting for tap/hold behaviour, keystrokes longer than tap_interval (ms) are holds */
};

struct key_actions_return_t {
	u8 data_present; /* if there is data returned */
	u8 keycodes[6]; /* keycode array for hid keyboard report */
	u8 mouse_buttons; /* mouse button state */
};

struct key_actions_return_t key_actions_update(struct key_actions_config_t config);
void key_actions_load_layer(struct key_actions_config_t config, u8 layer_no);
