/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "hal/spi.h"
#include "platform/samx7x/qspi.h"
#include "util/types.h"

struct spi_hal_t spi_hal_init_qspi(struct qspi_device_t *drv_data);
