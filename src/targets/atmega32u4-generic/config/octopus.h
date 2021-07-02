/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

/* clang-format off */

/* Clock Config */

#define F_CPU 16000000UL

/* Sensor Config */

#define SENSOR_ENABLED
#define SENSOR_DRIVER               PIXART_PMW

#define SENSOR_FIRMWARE_BLOB        pmw3360_blob

//#define SENSOR_MOTION_IO            { .port = , .pin =  }

#define SENSOR_INTERFACE            SPI_INTERFACE_1

#define SENSOR_INTERFACE_SPEED      10000000

/* o -> active low, 1 -> active high */
#define SENSOR_INTERFACE_CS_POL     0
#define SENSOR_INTERFACE_CS_IO      { .port = GPIO_PORT_B, .pin = 6 }

#define SENSOR_INTERFACE_SCK_IO     { .port = GPIO_PORT_B, .pin = 1 }
#define SENSOR_INTERFACE_MISO_IO    { .port = GPIO_PORT_B, .pin = 3 }
#define SENSOR_INTERFACE_MOSI_IO    { .port = GPIO_PORT_B, .pin = 2 }

/* IO config */
#define MB1_IO                      { .port = GPIO_PORT_F, .pin = 5 }
#define MB1_N_IO                    { .port = GPIO_PORT_F, .pin = 1 }
#define MB2_IO                      { .port = GPIO_PORT_F, .pin = 6 }
#define MB2_N_IO                    { .port = GPIO_PORT_F, .pin = 7 }
#define MB3_IO                      { .port = GPIO_PORT_D, .pin = 4 }
#define MB3_N_IO                    { .port = GPIO_PORT_D, .pin = 5 }
#define MB4_IO                      { .port = GPIO_PORT_D, .pin = 6 }
#define MB4_N_IO                    { .port = GPIO_PORT_D, .pin = 7 }
#define MB5_IO                      { .port = GPIO_PORT_B, .pin = 4 }
#define MB5_N_IO                    { .port = GPIO_PORT_B, .pin = 5 }

#define WHEEL_A_IO                  { .port = GPIO_PORT_D, .pin = 2 }
#define WHEEL_B_IO                  { .port = GPIO_PORT_D, .pin = 3 }
