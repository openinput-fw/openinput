/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

/* clang-format off */

/* General Config */

#define __SAMS70J19B__

/* Clock Config */

#define EXTERNAL_CLOCK_VALUE 12000000UL

/* Sensor Config */
#define SENSOR_ENABLED
#define SENSOR_DRIVER               PIXART_PMW

#define SENSOR_MOTION_IO            { .port = PIO_PORT_B, .pin = 3 }

#define SENSOR_INTERFACE_SPEED      1000000

/* o -> active low, 1 -> active high */
#define SENSOR_INTERFACE_CS_POL     0
#define SENSOR_INTERFACE_CS_IO      { .port = PIO_PORT_A, .pin = 10 }

#define SENSOR_INTERFACE_MISO_IO    { .port = PIO_PORT_A, .pin = 12 }
#define SENSOR_INTERFACE_MOSI_IO    { .port = PIO_PORT_A, .pin = 13 }
#define SENSOR_INTERFACE_SCK_IO     { .port = PIO_PORT_A, .pin = 14 }
