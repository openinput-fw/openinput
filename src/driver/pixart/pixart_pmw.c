/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "driver/pixart/pixart_pmw.h"

struct motion_burst_t {
	u8 motion;
	u8 observation;
	s16 dx;
	s16 dy;
} __attribute__((packed));

u8 pixart_pmw_read(struct pixart_pmw_driver_t driver, uint8_t address)
{
	driver.spi_hal.select(driver.spi_hal, 1);

	driver.spi_hal.transfer(driver.spi_hal, address & 0x7F); /* 7 bit address + read bit(0) */

	driver.ticks_hal.delay_us(120); /* Tsrad */

	u8 value = driver.spi_hal.transfer(driver.spi_hal, 0x00);

	driver.spi_hal.select(driver.spi_hal, 0);

	driver.ticks_hal.delay_us(160);

	return value;
}

void pixart_pmw_write(struct pixart_pmw_driver_t driver, uint8_t address, uint8_t value)
{
	driver.spi_hal.select(driver.spi_hal, 1);

	driver.spi_hal.transfer(driver.spi_hal, address | 0x80); /* 7 bit address + write bit(1) */

	driver.ticks_hal.delay_us(120); /* Tsrad */

	driver.spi_hal.transfer(driver.spi_hal, value);

	driver.spi_hal.select(driver.spi_hal, 0);

	driver.ticks_hal.delay_us(160);
}

struct motion_burst_t pixart_pmw_read_motion_burst(struct pixart_pmw_driver_t driver)
{
	struct motion_burst_t motion_burst;

	driver.spi_hal.select(driver.spi_hal, 1);

	driver.spi_hal.transfer(driver.spi_hal, PIXART_PMW_REG_BURST); /* 7 bit address + read bit(0) */

	driver.ticks_hal.delay_us(35); /* Tsrad_motbr */

	for (u8 i = 0; i < sizeof(motion_burst); i++)
		((u8 *) &motion_burst)[i] = driver.spi_hal.transfer(driver.spi_hal, 0x00);

	driver.spi_hal.select(driver.spi_hal, 0);

	return motion_burst;
}

struct pixart_pmw_driver_t pixart_pmw_init(u8 *firmware, struct spi_hal_t spi_hal, struct ticks_hal_t ticks_hal)
{
	struct pixart_pmw_driver_t driver = {};

	driver.spi_hal = spi_hal;
	driver.ticks_hal = ticks_hal;

	if (!firmware)
		return driver;

	spi_hal.select(spi_hal, 0);
	ticks_hal.delay_ms(1);

	/* Software Reset */
	pixart_pmw_write(driver, PIXART_PMW_REG_PWR_UP_RST, PIXART_PMW_RESET_CMD);
	ticks_hal.delay_ms(50);

	/* Check Known ID */
	u8 pid = pixart_pmw_read(driver, PIXART_PMW_REG_PID);
	if (pid != PIXART_PMW_PID_PMW3360 && pid != PIXART_PMW_PID_PMW3330 && pid != PIXART_PMW_PID_PMW3389)
		return driver;

	pixart_pmw_write(driver, PIXART_PMW_REG_CONFIG2, 0x00); /* Clear REST enable bit */

	/* Upload firmware blob */
	pixart_pmw_write(driver, PIXART_PMW_REG_SROM_EN, PIXART_PMW_SROM_DWNLD_CMD); /* Initialize SROM download */

	ticks_hal.delay_ms(15);

	pixart_pmw_write(driver, PIXART_PMW_REG_SROM_EN, PIXART_PMW_SROM_DWNLD_START_CMD); /* Start SROM download */

	spi_hal.select(spi_hal, 1);

	spi_hal.transfer(spi_hal, PIXART_PMW_REG_SROM_BURST | 0x80); /* 7 bit srom_burst address + write bit(1) */
	ticks_hal.delay_us(15);

	for (size_t b = 0; b < 4094; b++) /* write firmware image (4094 bytes) */
	{
		spi_hal.transfer(spi_hal, firmware[b]);
		ticks_hal.delay_us(15);
	}

	spi_hal.select(spi_hal, 0);

	ticks_hal.delay_ms(2); /* Tbexit */
	/* End upload firmware blob */

	u8 srom_id = pixart_pmw_read(driver, PIXART_PMW_REG_SROM_ID); /* read srom firmware id */
	if (!srom_id)
		return driver;

	pixart_pmw_write(driver, PIXART_PMW_REG_SROM_EN, PIXART_PMW_SROM_CRC_CMD); /* initialize CRC check */

	ticks_hal.delay_ms(15); /* Wait for CRC */

	u16 srom_crc = 0;
	srom_crc |= (pixart_pmw_read(driver, PIXART_PMW_REG_DOUT_H) << 8);
	srom_crc |= pixart_pmw_read(driver, PIXART_PMW_REG_DOUT_L);

	/* TODO: check CRC */
	if (!srom_crc)
		return driver;

	if (!(pixart_pmw_read(driver, PIXART_PMW_REG_OBSERVATION) & PIXART_PMW_SROM_RUN)) /* check SROM running bit */
		return driver;

	pixart_pmw_write(driver, PIXART_PMW_REG_CONFIG2, 0x00); /* clear REST enable bit */

	pixart_pmw_write(driver, PIXART_PMW_REG_BURST, 0x01);

	ticks_hal.delay_ms(10);

	pixart_pmw_write(driver, PIXART_PMW_REG_OBSERVATION, 0x00); /* clear observation register */

	pixart_pmw_write(driver, PIXART_PMW_REG_BURST, 0x01);

	pixart_pmw_read_motion_burst(driver);

	return driver;
}

void pixart_pmw_read_motion(struct pixart_pmw_driver_t *driver)
{
	struct motion_burst_t motion_burst = pixart_pmw_read_motion_burst(*driver);

	driver->deltas.dx += motion_burst.dx;
	driver->deltas.dy += motion_burst.dy;

	driver->motion_flag = 0;
}

void pixart_pmw_motion_event(struct pixart_pmw_driver_t *driver)
{
	driver->motion_flag++;
}

struct deltas_t pixart_pmw_get_deltas(struct pixart_pmw_driver_t *driver)
{
	struct deltas_t deltas = driver->deltas;

	driver->deltas.dx = 0;
	driver->deltas.dy = 0;

	return deltas;
}
