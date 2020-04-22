// SPDX-License-Identifier: MIT

#ifndef _TARGET_H_
#define _TARGET_H_

#include "tusb.h"

//--------------------------------------------------------------------
// Data types
//--------------------------------------------------------------------

typedef struct {
    int8_t dx;
    int8_t dy;
} deltas_t;

//--------------------------------------------------------------------
// Target Porting API
//--------------------------------------------------------------------

void target_tasks();

uint64_t target_ticks();

void target_delay_ms(uint16_t ticks);

//--------------------------------------------------------------------
// Outputs API
//--------------------------------------------------------------------

void target_leds_write(uint8_t r, uint8_t g, uint8_t b);

//--------------------------------------------------------------------
// Inputs API
//--------------------------------------------------------------------

deltas_t target_sensor_deltas_get();

uint8_t target_mouse_btns_get();

int8_t target_wheel_get();

#endif // _TARGET_H_