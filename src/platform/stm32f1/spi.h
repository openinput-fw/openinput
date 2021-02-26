/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#pragma once

#include "platform/stm32f1/gpio.h"
#include "util/types.h"

#define SPI_MSB_FIRST 0
#define SPI_LSB_FIRST 1

enum spi_mode {
	SPI_MODE0 = 0,
	SPI_MODE1 = 1,
	SPI_MODE2 = 2,
	SPI_MODE3 = 3,
};

enum spi_interface_no {
	SPI_INTERFACE_1,
#if defined(SPI2)
	SPI_INTERFACE_2,
#endif
#if defined(SPI3)
	SPI_INTERFACE_3,
#endif
};

struct spi_device_t {
	struct gpio_pin_t cs_gpio;
	u8 cs_inverted;
	void *interface;
};

void spi_init_interface(enum spi_interface_no interface_no, enum spi_mode mode, u32 frequency, u8 bit_order);
struct spi_device_t spi_init_device(enum spi_interface_no interface_no, struct gpio_pin_t cs_gpio, u8 cs_inverted);
void spi_select(struct spi_device_t device, u8 state);
u8 spi_transfer_byte(struct spi_device_t device, const u8 data);
void spi_transfer(struct spi_device_t device, const u8 *src, u32 size, u8 *dst);
