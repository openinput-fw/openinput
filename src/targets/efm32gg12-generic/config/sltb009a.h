/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

/* clang-format off */

/* General Config */

#define EFM32GG12B810F1024GM64

/* Clock Config */

#define EXTERNAL_CLOCK_VALUE 50000000UL

/* Energy Management Unit Config */
#define DCDC_ENABLE
#define DCDC_VOLTAGE    1800.f
#define DCDC_ACTIVE_I   50.f
#define DCDC_SLEEP_I    100.f
#define DCDC_REVERSE_I  0

#define REGULATOR_OUT   3300.f
