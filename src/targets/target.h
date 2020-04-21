/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 openinput (https://github.com/openinput-fw)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

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