/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/gpio.h"

u64 gpio_mode(struct gpio_port_t port)
{
	u64 mode = 0;

	for (size_t i = 0; i < 8; i++)
		mode |= port.MODE_CNF[i] << (i * 4);

	return mode;
}

void gpio_apply_config(struct gpio_config_t config)
{
	/* reset port A */
	RCC->APB2RSTR |= RCC_APB2RSTR_IOPARST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPARST;
	/* configure port A */
	GPIOA->CRL = gpio_mode(config.port[GPIO_PORT_A]) & 0x00FF;
	GPIOA->CRH = (gpio_mode(config.port[GPIO_PORT_A]) & 0xFF00) >> 32;
	GPIOA->ODR = config.port[GPIO_PORT_A].ODR;

	/* reset port B */
	RCC->APB2RSTR |= RCC_APB2RSTR_IOPBRST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPBRST;
	/* configure port B */
	GPIOB->CRL = gpio_mode(config.port[GPIO_PORT_B]) & 0x00FF;
	GPIOB->CRH = (gpio_mode(config.port[GPIO_PORT_B]) & 0xFF00) >> 32;
	GPIOB->ODR = config.port[GPIO_PORT_B].ODR;

	/* reset port C */
	RCC->APB2RSTR |= RCC_APB2RSTR_IOPCRST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPCRST;
	/* configure port C */
	GPIOC->CRL = gpio_mode(config.port[GPIO_PORT_C]) & 0x00FF;
	GPIOC->CRH = (gpio_mode(config.port[GPIO_PORT_C]) & 0xFF00) >> 32;
	GPIOC->ODR = config.port[GPIO_PORT_C].ODR;

	/* reset port D */
	RCC->APB2RSTR |= RCC_APB2RSTR_IOPDRST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPDRST;
	/* configure port D */
	GPIOC->CRL = gpio_mode(config.port[GPIO_PORT_D]) & 0x00FF;
	GPIOC->CRH = (gpio_mode(config.port[GPIO_PORT_D]) & 0xFF00) >> 32;
	GPIOC->ODR = config.port[GPIO_PORT_D].ODR;
}

/* led setup */

void gpio_led_setup(struct gpio_config_t *config, struct gpio_pin_t led)
{
	config->port[led.port].MODE_CNF[led.pin] = GPIO_MODE_OUTPUT_10MHZ | GPIO_CNF_OUTPUT_ALTERNATE_PUSH_PULL;
}

void gpio_rgb_led_setup(struct gpio_config_t *config, struct gpio_rgb_led_pin_t led)
{
	gpio_led_setup(config, led.r);
	gpio_led_setup(config, led.g);
	gpio_led_setup(config, led.b);
}
