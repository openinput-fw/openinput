/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include "platform/stm32f1/gpio.h"
#include "platform/stm32f1/rcc.h"

#define STM32F103xB

#define EXTERNAL_CLOCK_VALUE STM32F1_CLOCK_8MHZ

static const struct gpio_rgb_led_pin_t leds[] = {}
