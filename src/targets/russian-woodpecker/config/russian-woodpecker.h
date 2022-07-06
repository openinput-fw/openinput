/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

/* clang-format off */

/* General Config */

#define __SAM3U2C__

/* Clock Config */
#define EXTERNAL_CLOCK_VALUE 12000000UL

/* LED */
#define LED_R  { .port = PIO_PORT_A, .pin = 30 }
#define LED_G  { .port = PIO_PORT_A, .pin = 1 }
#define LED_B  { .port = PIO_PORT_B, .pin = 5 }
