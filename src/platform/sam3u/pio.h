/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

/* ports */
enum pio_port_t {
	PIO_PORT_A = 0,
	PIO_PORT_B = 1,
#if defined(PIOC)
	PIO_PORT_C = 2,
#endif
};

struct pio_pin_t {
	u8 port;
	u8 pin;
};

enum pio_mux_t {
	PIO_MUX_A = 0,
	PIO_MUX_B = 1,
};

enum pio_pull_t {
	PIO_PULL_NONE = 0x0,
	PIO_PULL_UP = 0x1,
};

enum pio_direction_t {
	PIO_DIRECTION_OUT = 0,
	PIO_DIRECTION_IN = 1,
};

enum pio_config_flags_t {
	PIO_OPEN_DRAIN = 0x4, // bit 3
	PIO_FILTER = 0x10, // bit 5
	PIO_PERIPHERAL_CTRL = 0x40, // bit 7
};

void pio_init();

void pio_config(
	struct pio_pin_t pin,
	enum pio_direction_t direction,
	u8 out_data,
	enum pio_pull_t pull,
	enum pio_mux_t mux,
	enum pio_config_flags_t config);

void pio_peripheral_mux(struct pio_pin_t pin, enum pio_mux_t mux);

void pio_peripheral_control(struct pio_pin_t pin, u8 enable);

void pio_direction(struct pio_pin_t pin, enum pio_direction_t direction);

void pio_pull(struct pio_pin_t pin, enum pio_pull_t pull);

void pio_set(struct pio_pin_t pin, u8 state);

u8 pio_get(struct pio_pin_t pin);
