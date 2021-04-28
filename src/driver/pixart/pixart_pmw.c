/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include "driver/pixart/pixart_pmw.h"

/*
 * Pixar PMW33xx driver
 *
 * currently supports pmw3360, truemove3 and the pmw3389
 *
 * the pmw3330 is NOT supported, it does not share many
 * of the same registers, does not need SROM upload at
 * startup and is lacking some important features like
 * motion burst
 */

/* General Registers */
#define PIXART_PMW_REG_PID	  0x00
#define PIXART_PMW_REG_REV_ID	  0x01
#define PIXART_PMW_REG_PWR_UP_RST 0x3A

#define PIXART_PMW_PID_PMW3360	 0x42
#define PIXART_PMW_PID_TRUEMOVE3 PIXART_PMW_PID_PMW3360 /* TRUEMOVE 3 Shares the same PID as PMW3360*/
#define PIXART_PMW_PID_PMW3330	 0x45
#define PIXART_PMW_PID_PMW3389	 0x47

/* Motion Registers */
#define PIXART_PMW_REG_MOTION	 0x02
#define PIXART_PMW_REG_DELTA_X_L 0x03
#define PIXART_PMW_REG_DELTA_X_H 0x04
#define PIXART_PMW_REG_DELTA_Y_L 0x05
#define PIXART_PMW_REG_DELTA_Y_H 0x06
#define PIXART_PMW_REG_BURST	 0x50

/* Motion bits */
#define PIXART_PMW_MOTION	(1 << 8)
#define PIXART_PMW_OPMODE_RUN	(0)
#define PIXART_PMW_OPMODE_REST1 (0b01 << 1)
#define PIXART_PMW_OPMODE_REST2 (0b10 << 1)
#define PIXART_PMW_OPMODE_REST3 (0b11 << 1)

/* SROM Registers */
#define PIXART_PMW_REG_SROM_EN	  0x13
#define PIXART_PMW_REG_SROM_ID	  0x2A
#define PIXART_PMW_REG_SROM_BURST 0x62

/* SROM CMDs */
#define PIXART_PMW_SROM_CRC_CMD		0x15
#define PIXART_PMW_SROM_DWNLD_CMD	0x1D
#define PIXART_PMW_SROM_DWNLD_START_CMD 0x18

/* CPI Registers */
#define PIXART_PMW3360_REG_CPI	 0x0F
#define PIXART_PMW3389_REG_CPI_L 0x0E
#define PIXART_PMW3389_REG_CPI_H 0x0F

/* Config Registers */
#define PIXART_PMW_REG_CONFIG2	   0x10
#define PIXART_PMW_REG_OBSERVATION 0x24
#define PIXART_PMW_REG_DOUT_L	   0x25
#define PIXART_PMW_REG_DOUT_H	   0x26

/* Config2 Bits */
#define PIXART_PMW_RESTEN  0x20
#define PIXART_PMW_RPT_MOD 0x04

/* Observation Bits */
#define PIXART_PMW_SROM_RUN 0x40

/* power up reset cmd */
#define PIXART_PMW_RESET_CMD 0x5A

struct motion_burst_t {
	u8 motion;
	u8 observation;
	s16 dx;
	s16 dy;
} __attribute__((packed));

u8 pixart_pmw_read(struct pixart_pmw_driver_t driver, u8 address)
{
	driver.spi_hal.select(driver.spi_hal, 1);

	driver.spi_hal.transfer(driver.spi_hal, address & 0x7F); /* 7 bit address + read bit(0) */

	driver.ticks_hal.delay_us(120); /* Tsrad */

	u8 value = driver.spi_hal.transfer(driver.spi_hal, 0x00);

	driver.spi_hal.select(driver.spi_hal, 0);

	driver.ticks_hal.delay_us(160);

	return value;
}

void pixart_pmw_write(struct pixart_pmw_driver_t driver, u8 address, u8 value)
{
	driver.spi_hal.select(driver.spi_hal, 1);

	driver.spi_hal.transfer(driver.spi_hal, address | 0x80); /* 7 bit address + write bit(1) */

	driver.ticks_hal.delay_us(120); /* Tsrad */

	driver.spi_hal.transfer(driver.spi_hal, value);

	driver.spi_hal.select(driver.spi_hal, 0);

	driver.ticks_hal.delay_us(160);
}

void pixart_pmw_upload_srom(struct pixart_pmw_driver_t driver, u8 *firmware)
{
	pixart_pmw_write(driver, PIXART_PMW_REG_SROM_EN, PIXART_PMW_SROM_DWNLD_CMD); /* Initialize SROM download */

	driver.ticks_hal.delay_ms(15);

	pixart_pmw_write(driver, PIXART_PMW_REG_SROM_EN, PIXART_PMW_SROM_DWNLD_START_CMD); /* Start SROM download */

	driver.spi_hal.select(driver.spi_hal, 1);

	driver.spi_hal.transfer(
		driver.spi_hal, PIXART_PMW_REG_SROM_BURST | 0x80); /* 7 bit srom_burst address + write bit(1) */
	driver.ticks_hal.delay_us(15);

	for (size_t b = 0; b < 4094; b++) /* write firmware image (4094 bytes) */
	{
		driver.spi_hal.transfer(driver.spi_hal, firmware[b]);
		driver.ticks_hal.delay_us(15);
	}

	driver.spi_hal.select(driver.spi_hal, 0);

	driver.ticks_hal.delay_ms(2); /* Tbexit */
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
		goto failed_init;

	spi_hal.select(spi_hal, 0);
	ticks_hal.delay_ms(1);

	/* Software Reset */
	pixart_pmw_write(driver, PIXART_PMW_REG_PWR_UP_RST, PIXART_PMW_RESET_CMD);
	ticks_hal.delay_ms(50);

	/* Check Known and supported ID */
	u8 pid = pixart_pmw_read(driver, PIXART_PMW_REG_PID);
	if (pid != PIXART_PMW_PID_PMW3360 && pid != PIXART_PMW_PID_PMW3389)
		goto failed_init;

	driver.pid = pid;

	pixart_pmw_write(driver, PIXART_PMW_REG_CONFIG2, 0x00); /* Clear REST enable bit */

	pixart_pmw_upload_srom(driver, firmware); /* Upload firmware blob */

	if (!(pixart_pmw_read(driver, PIXART_PMW_REG_OBSERVATION) & PIXART_PMW_SROM_RUN)) /* check SROM running bit */
		goto failed_init;

	u8 srom_id = pixart_pmw_read(driver, PIXART_PMW_REG_SROM_ID); /* read srom firmware id */
	if (!srom_id)
		goto failed_init;

	pixart_pmw_write(driver, PIXART_PMW_REG_CONFIG2, 0x00); /* clear REST enable bit */

	pixart_pmw_write(driver, PIXART_PMW_REG_BURST, 0x01);

	pixart_pmw_read_motion_burst(driver); /* Read motion and discard the data*/

	return driver;

failed_init:
	driver.pid = 0; /* pid == 0 means failed initialization */
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

void pixart_pmw_set_cpi(struct pixart_pmw_driver_t driver, u16 cpi)
{
	switch (driver.pid) {
		case PIXART_PMW_PID_PMW3360:
			if (cpi >= 100 && cpi <= 12000)
				pixart_pmw_write(
					driver, PIXART_PMW3360_REG_CPI, ((cpi / 100) - 1)); /* 100-12000, 100 cpi LSb */
			break;

		case PIXART_PMW_PID_PMW3389:
			if (cpi >= 50 && cpi <= 16000) {
				cpi /= 50;
				pixart_pmw_write(driver, PIXART_PMW3389_REG_CPI_L, cpi & 0xFF); /* 50-16000, 50 cpi LSb */
				pixart_pmw_write(driver, PIXART_PMW3389_REG_CPI_H, cpi >> 8); /* 50-16000, 50 cpi LSb */
			}
			break;
	}
}
