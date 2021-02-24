/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/gpio.h"
#include "platform/stm32f1/rcc.h"
#include "platform/stm32f1/spi.h"
#include "util/types.h"

void spi_init_interface(enum spi_interface_no interface_no, enum spi_mode mode, u32 frequency, u8 bit_order)
{
	struct rcc_clock_tree_t clock_tree = rcc_get_clock_tree();
	u32 src_clck;
	SPI_TypeDef *interface;

	switch (interface_no) {
		case SPI_INTERFACE_1:
			RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST; /* Reset SPI1 peripheral */
			RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;

			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; /* Enable SPI1 peripheral clock */

			src_clck = clock_tree.apb2_clock_freq;

			interface = SPI1;
			break;

#if defined(SPI2)
		case SPI_INTERFACE_2:
			RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST; /* Reset SPI2 peripheral */
			RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;

			RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; /* Enable SPI2 peripheral clock */

			src_clck = clock_tree.apb1_clock_freq;

			interface = SPI2;
			break;
#endif

#if defined(SPI3)
		case SPI_INTERFACE_3:
			RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST; /* Reset SPI3 peripheral */
			RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;

			RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; /* Enable SPI2 peripheral clock */

			src_clck = clock_tree.apb1_clock_freq;

			interface = SPI3;
			break;
#endif
	}

	/* Calculate clock divider */
	u32 divider = src_clck / frequency;

	if (divider <= 2)
		divider = 0; /* divide by 2 */
	else if (divider <= 4)
		divider = 1; /* divide by 4 */
	else if (divider <= 8)
		divider = 2; /* divide by 8 */
	else if (divider <= 16)
		divider = 3; /* divide by 16 */
	else if (divider <= 32)
		divider = 4; /* divide by 32 */
	else if (divider <= 64)
		divider = 5; /* divide by 64 */
	else if (divider <= 128)
		divider = 6; /* divide by 128 */
	else
		divider = 7; /* divide by 256 */

	/* configure SPI */
	interface->I2SCFGR = 0x0000; /* SPI mode enabled I2S mode disabled*/

	interface->CR2 = 0x00000000;
	interface->CR1 = mode | SPI_CR1_MSTR | (divider << SPI_CR1_BR_Pos) | SPI_CR1_SPE |
			 (bit_order << SPI_CR1_LSBFIRST_Pos) | SPI_CR1_SSI | SPI_CR1_SSM;
}

struct spi_device_t spi_init_device(enum spi_interface_no interface_no, struct gpio_pin_t cs_gpio, u8 cs_inverted)
{
	void *interface;

	switch (interface_no) {
		case SPI_INTERFACE_1:
			interface = SPI1;
			break;

#if defined(SPI2)
		case SPI_INTERFACE_2:
			interface = SPI2;
			break;
#endif

#if defined(SPI3)
		case SPI_INTERFACE_3:
			interface = SPI3;
			break;
#endif
	}

	struct spi_device_t device = {
		.interface = interface,
		.cs_gpio = cs_gpio,
		.cs_inverted = !!cs_inverted,
	};
	return device;
}

void spi_select(struct spi_device_t device, u8 state)
{
	/* state 1 = selected, active low unless cs_inverted is set */
	gpio_set(device.cs_gpio, (!state) ^ device.cs_inverted);
}

u8 spi_transfer_byte(struct spi_device_t device, const u8 data)
{
	while (!(((SPI_TypeDef *) device.interface)->SR & SPI_SR_TXE)) continue;

	((SPI_TypeDef *) device.interface)->DR = data;

	while (!(((SPI_TypeDef *) device.interface)->SR & SPI_SR_RXNE)) continue;

	return (u8)((SPI_TypeDef *) device.interface)->DR;
}

void spi_transfer(struct spi_device_t device, const u8 *src, u32 size, u8 *dst)
{
	if (src) {
		while (size--) {
			if (dst)
				*(dst++) = spi_transfer_byte(device, *(src++));
			else
				spi_transfer_byte(device, *(src++));
		}
	} else if (dst) {
		while (size--) *(dst++) = spi_transfer_byte(device, 0x00);
	}
}
