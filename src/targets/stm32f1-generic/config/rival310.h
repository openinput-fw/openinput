/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

/* clang-format off */

/* General Config */

#define STM32F103x6

/* Clock Config */

#define EXTERNAL_CLOCK_VALUE STM32F1_CLOCK_12MHZ

/* USB Config */

#define USB_VID 0x1D50 /* Attributed by openmoko */
#define USB_PID 0x616A /* Attributed by openmoko */

/*
 * The USB standard requires a 1.5k pullup resistor on D+
 * Bluepill boards are known to come with the wrong value resistor here (R10)
 * it is usually populated with a 10k or a 4.7k resistor and should be replaced with a 1.5k
 */
#define USB_DP_PU_IO                { .port = GPIO_PORT_A, .pin = 15 }

/* Sensor Config */
#define SENSOR_ENABLED
#define SENSOR_DRIVER               PIXART_PMW

#define SENSOR_FIRMWARE_BLOB        truemove3_blob

#define SENSOR_MOTION_IO            { .port = GPIO_PORT_A, .pin = 3 }

#define SENSOR_INTERFACE            SPI_INTERFACE_1

#define SENSOR_INTERFACE_SPEED      10000000

/* o -> active low, 1 -> active high */
#define SENSOR_INTERFACE_CS_POL     0
#define SENSOR_INTERFACE_CS_IO      { .port = GPIO_PORT_A, .pin = 4 }

#define SENSOR_INTERFACE_SCK_IO     { .port = GPIO_PORT_A, .pin = 5 }
#define SENSOR_INTERFACE_MISO_IO    { .port = GPIO_PORT_A, .pin = 6 }
#define SENSOR_INTERFACE_MOSI_IO    { .port = GPIO_PORT_A, .pin = 7 }
