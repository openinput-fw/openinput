/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <em_device.h>

#include "platform/efm32gg/gpio.h"
#include "util/data.h"

/* initiates all pins to a safe known state, disabled */
void gpio_init_config(struct gpio_config_t *config)
{
	for (u8 port = 0; port < 6; port++) {
		config->port[port].dout = 0;
		for (u8 pin = 0; pin < 16; pin++) {
			config->port[port].pin_cfg[pin] = GPIO_MODE_DISABLED;
		}
	}
}

void gpio_apply_config(struct gpio_config_t config)
{
	CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;

	for (u8 port = 0; port < 6; port++) {
		GPIO->P[port].CTRL = GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG | (5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT) |
				     GPIO_P_CTRL_DRIVESTRENGTH_STRONG | (5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
		GPIO->P[port].MODEL = 0;
		GPIO->P[port].MODEH = 0;
		for (u8 pin = 0; pin < 8; pin++) {
			GPIO->P[port].MODEL |= config.port[port].pin_cfg[pin] << (pin << 2);
		}
		for (u8 pin = 8; pin < 16; pin++) {
			GPIO->P[port].MODEH |= config.port[port].pin_cfg[pin] << ((pin - 8) << 2);
		}
		GPIO->P[port].DOUT = config.port[port].dout;
	}

	// Debugger Route
	GPIO->ROUTEPEN &= ~(GPIO_ROUTEPEN_TDIPEN | GPIO_ROUTEPEN_TDOPEN); // Disable unused JTAG pins (SWD is expected)
}

void gpio_setup_pin(struct gpio_config_t *config, struct gpio_pin_t pin, enum gpio_mode mode, u8 out)
{
	config->port[pin.port].pin_cfg[pin.pin] = mode;
	config->port[pin.port].dout = (config->port[pin.port].dout & ~BIT(pin.pin)) | (!!out) << pin.pin;
}

void gpio_set(struct gpio_pin_t pin, u8 out)
{
	GPIO->P[pin.port].DOUT = (GPIO->P[pin.port].DOUT & ~BIT(pin.pin)) | (!!out << pin.pin);
}

void gpio_toggle(struct gpio_pin_t pin)
{
	GPIO->P[pin.port].DOUT ^= BIT(pin.pin);
}

u8 gpio_get(struct gpio_pin_t pin)
{
	return !!(GPIO->P[pin.port].DIN & BIT(pin.pin));
}
