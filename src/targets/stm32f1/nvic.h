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

#ifndef __NVIC_H__
#define __NVIC_H__

//--------------------------------------------------------------------
// Interrupt utility definitions
//--------------------------------------------------------------------

#define IRQ_GROUP_BITS          3
#define IRQ_SUB_GROUP_BITS      1
#define IRQ_SET_PRIO(i, g, s)   NVIC->IP[i] = ((((g) & ((1 << IRQ_GROUP_BITS) - 1)) << IRQ_SUB_GROUP_BITS) | ((s) & ((1 << IRQ_SUB_GROUP_BITS) - 1))) << (8 - __NVIC_PRIO_BITS)
#define IRQ_GET_PRIO(i)         (NVIC->IP[i] >> (8 - __NVIC_PRIO_BITS))
#define IRQ_ENABLE(i)           NVIC->ISER[i >> 5] = 1 << (i & 0x1F)
#define IRQ_DISABLE(i)          NVIC->ICER[i >> 5] = 1 << (i & 0x1F)
#define IRQ_SET(i)              NVIC->ISPR[i >> 5] = 1 << (i & 0x1F)
#define IRQ_CLEAR(i)            NVIC->ICPR[i >> 5] = 1 << (i & 0x1F)
#define IRQ_STATUS(i)           (NVIC->ISER[i >> 5] & (1 << (i & 0x1F)))
#define IRQ_PENDING(i)          (NVIC->ISPR[i >> 5] & (1 << (i & 0x1F)))

#endif