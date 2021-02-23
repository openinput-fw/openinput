/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2020 Rafael Silva <silvagracarafael@gmail.com>
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#pragma once

/* CNF input values*/
#define GPIO_CNF_INPUT_ANALOG	(0b00 << 2)
#define GPIO_CNF_INPUT_FLOATING (0b01 << 2)
#define GPIO_CNF_INPUT_PULL	(0b10 << 2)
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
	u8 port;
	u8 pin;
};

struct gpio_port_config_t {
	u8 MODE_CNF[16];
	u16 ODR;
};

struct gpio_config_t {
	struct gpio_port_config_t port[4];
};

void gpio_apply_config(struct gpio_config_t config);
void gpio_setup_pin(struct gpio_config_t *config, struct gpio_pin_t pin, u8 mode_cnf, u8 out);
void gpio_set(struct gpio_pin_t pin, u8 out);
void gpio_toggle(struct gpio_pin_t pin);
u8 gpio_get(struct gpio_pin_t pin);
