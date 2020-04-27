// SPDX-License-Identifier: MIT

#ifndef _LEDS_H_
#define _LEDS_H_

#include "config.h"
#include <stm32f1xx.h>
#include "rcc.h"

//--------------------------------------------------------------------
// Function declarations
//--------------------------------------------------------------------

void led_logo_write(uint8_t r, uint8_t g, uint8_t b);
void led_wheel_write(uint8_t r, uint8_t g, uint8_t b);
void leds_init();

#endif // _LEDS_H_