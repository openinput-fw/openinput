/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2020 Rafael Silva <silvagracarafael@gmail.com>
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

#include "util/types.h"

/* CNF input values*/
#define GPIO_CNF_INPUT_ANALOG	(0b00 << 2)
#define GPIO_CNF_INPUT_FLOATING (0b01 << 2)
#define GPIO_CNF_INPUT_INPUT	(0b10 << 2)
/* CNF output values - */
#define GPIO_CNF_OUTPUT_GENERAL_PUSH_PULL    (0b00 << 2)
#define GPIO_CNF_OUTPUT_GENERAL_OPEN_DRAIN   (0b01 << 2)
#define GPIO_CNF_OUTPUT_ALTERNATE_PUSH_PULL  (0b10 << 2)
#define GPIO_CNF_OUTPUT_ALTERNATE_OPEN_DRAIN (0b11 << 2)
/* MODE values */
#define GPIO_MODE_INPUT	       0b00
#define GPIO_MODE_OUTPUT_10MHZ 0b01
#define GPIO_MODE_OUTPUT_2MHZ  0b10
#define GPIO_MODE_OUTPUT_50MHZ 0b11

/* ports */
#define GPIO_PORT_A 0
#define GPIO_PORT_B 1
#define GPIO_PORT_C 2
#define GPIO_PORT_D 3

struct gpio_pin_t {
	u32 port;
	u32 pin;
};

struct gpio_rgb_led_pin_t {
	struct gpio_pin_t r;
	struct gpio_pin_t g;
	struct gpio_pin_t b;
};

struct gpio_port_t {
	u8 MODE_CNF[16];
	u32 ODR;
};

struct gpio_config_t {
	struct gpio_port_t port[4];
};

u64 gpio_mode(struct gpio_port_t port);
void gpio_apply_config(struct gpio_config_t config);
void gpio_led_setup(struct gpio_config_t *config, struct gpio_pin_t led);
void gpio_rgb_led_setup(struct gpio_config_t *config, struct gpio_rgb_led_pin_t led);
