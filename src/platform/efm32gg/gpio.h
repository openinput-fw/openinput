/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

/* ports */
#define GPIO_PORT_A 0
#define GPIO_PORT_B 1
#define GPIO_PORT_C 2
#define GPIO_PORT_D 3
#define GPIO_PORT_E 4
#define GPIO_PORT_F 5

enum gpio_mode {
	GPIO_MODE_DISABLED = 0x0,
	GPIO_MODE_INPUT = 0x1,
	GPIO_MODE_INPUTPULL = 0x2,
	GPIO_MODE_INPUTPULLFILTER = 0x3,
	GPIO_MODE_PUSHPULL = 0x4,
	GPIO_MODE_PUSHPULLALT = 0x5,
	GPIO_MODE_WIREDOR = 0x6,
	GPIO_MODE_WIREDORPULLDOWN = 0x7,
	GPIO_MODE_WIREDAND = 0x8,
	GPIO_MODE_WIREDANDFILTER = 0x9,
	GPIO_MODE_WIREDANDPULLUP = 0xA,
	GPIO_MODE_WIREDANDPULLUPFILTER = 0xB,
	GPIO_MODE_WIREDANDALT = 0xC,
	GPIO_MODE_WIREDANDALTFILTER = 0xD,
	GPIO_MODE_WIREDANDALTPULLUP = 0xE,
	GPIO_MODE_WIREDANDALTPULLUPFILTER = 0xF,
};

struct gpio_pin_t {
	u8 port;
	u8 pin;
};

struct gpio_port_config_t {
	u8 pin_cfg[16];
	u32 dout;
};

struct gpio_config_t {
	struct gpio_port_config_t port[6];
};

void gpio_init_config(struct gpio_config_t *config);
void gpio_apply_config(struct gpio_config_t config);
void gpio_setup_pin(struct gpio_config_t *config, struct gpio_pin_t pin, enum gpio_mode mode, u8 out);
void gpio_set(struct gpio_pin_t pin, u8 out);
void gpio_toggle(struct gpio_pin_t pin);
u8 gpio_get(struct gpio_pin_t pin);
