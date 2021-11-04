/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "platform/samx7x/pio.h"
#include "util/types.h"

enum qspi_mode {
	QSPI_MODE0 = 0,
	QSPI_MODE1 = 1,
	QSPI_MODE2 = 2,
	QSPI_MODE3 = 3,
};

struct qspi_device_t {
	struct pio_pin_t cs_pio;
	u8 cs_inverted;
};

void qspi_init_interface(enum qspi_mode mode, u32 frequency);
struct qspi_device_t qspi_init_device(struct pio_pin_t cs_pio, u8 cs_inverted);
void qspi_select(struct qspi_device_t device, u8 state);
u8 qspi_transfer_byte(const u8 data);
void qspi_transfer(const u8 *src, u32 size, u8 *dst);
