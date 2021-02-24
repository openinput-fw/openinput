/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#pragma once

#include <stm32f1xx.h>

#include "platform/stm32f1/gpio.h"

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

struct spi_drv_data_t {
	struct gpio_pin_t cs_gpio;
	u8 cs_inverted;
	SPI_TypeDef *interface;
};

int spi_init(
	struct spi_drv_data_t *drv_data,
	enum spi_interface_no interface_no,
	enum spi_mode mode,
	u32 frequency,
	u8 bit_order,
	struct gpio_pin_t cs_gpio,
	u8 cs_inverted);
void spi_select(struct spi_drv_data_t drv_data, u8 state);
u8 spi_transfer_byte(struct spi_drv_data_t drv_data, const u8 data);
void spi_transfer(struct spi_drv_data_t drv_data, const u8 *src, u32 size, u8 *dst);
