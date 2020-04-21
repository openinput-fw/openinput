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
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include <stm32f1xx.h>
#include "utils.h"
#include "rcc.h"

//--------------------------------------------------------------------
// Definitions
//--------------------------------------------------------------------

// GPIO & EXTI helper macros
#define IO_MODE_IN          0x0
#define IO_MODE_OUT_10M     0x1
#define IO_MODE_OUT_2M      0x2
#define IO_MODE_OUT_50M     0x3
#define IO_CFG_IN_ANALOG    0x0
#define IO_CFG_IN_FLOAT     0x4
#define IO_CFG_IN_PULL      0x8
#define IO_CFG_OUT_GP_PP    0x0
#define IO_CFG_OUT_GP_OD    0x4
#define IO_CFG_OUT_AF_PP    0x8
#define IO_CFG_OUT_AF_OD    0xC
#define IO_MODE_CFG(p, m, c)    ((uint32_t)(m | c) << ((p & (uint32_t)7) * 4))
#define _IO_MODE_CFG(p, c)    ((uint32_t)(c) << ((p & (uint32_t)7) * 4))
#define IO_EXTI_PORTA       0x0
#define IO_EXTI_PORTB       0x1
#define IO_EXTI_PORTC       0x2
#define IO_EXTI_PORTD       0x3
#define IO_EXTI_PORTE       0x4
#define IO_EXTI_PORTF       0x5
#define IO_EXTI_PORTG       0x6
#define IO_EXTI_CFG(i, p)   ((uint32_t)p << ((i & (uint32_t)3) * 4))

//--------------------------------------------------------------------
// Data types
//--------------------------------------------------------------------

typedef struct{
    struct port_s {
        uint8_t MODE_CFG[16];
        uint32_t ODR;
    } port[4];
} gpio_init_t;

//--------------------------------------------------------------------
// Funcions declaration
//--------------------------------------------------------------------

void gpio_init();

#endif