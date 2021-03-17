/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 * SPDX-FileCopyrightText: 2021 Rafael Silva <silvagracarafael@gmail.com>
 */

#pragma once

#include "hal/spi.h"
#include "hal/ticks.h"
#include "util/types.h"

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
#define PIXART_PMW_MOTION	0x80
#define PIXART_PMW_OPMODE_RUN	0x00
#define PIXART_PMW_OPMODE_REST1 0x02
#define PIXART_PMW_OPMODE_REST2 0x04
#define PIXART_PMW_OPMODE_REST3 0x06

/* SROM Registers */
#define PIXART_PMW_REG_SROM_EN	  0x13
#define PIXART_PMW_REG_SROM_ID	  0x2A
#define PIXART_PMW_REG_SROM_BURST 0x62

/* SROM CMDs */
#define PIXART_PMW_SROM_CRC_CMD		0x15
#define PIXART_PMW_SROM_DWNLD_CMD	0x1D
#define PIXART_PMW_SROM_DWNLD_START_CMD 0x18

/* Config Registers */
#define PIXART_PMW_REG_CONFIG1	   0x0F
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

struct deltas_t {
	s16 dx;
	s16 dy;
};

struct pixart_pmw_driver_t {
	u8 motion_flag;
	struct deltas_t deltas;
	struct spi_hal_t spi_hal;
	struct ticks_hal_t ticks_hal;
};

struct pixart_pmw_driver_t pixart_pmw_init(u8 *firmware, struct spi_hal_t spi_hal, struct ticks_hal_t ticks_hal);

void pixart_pmw_read_motion(struct pixart_pmw_driver_t *driver);

void pixart_pmw_motion_event(struct pixart_pmw_driver_t *driver);

struct deltas_t pixart_pmw_get_deltas(struct pixart_pmw_driver_t *driver);
