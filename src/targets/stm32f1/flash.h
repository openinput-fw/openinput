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

#ifndef __FLASH_H__
#define __FLASH_H__

#include <stm32f1xx.h>

//--------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------

#define FLASH_SIZE          (((uint32_t)*(volatile uint16_t *)0x1FFFF7E0) << 10)
#define FLASH_PAGE_SIZE     2048
#define FLASH_PAGE_COUNT    (FLASH_SIZE / FLASH_PAGE_SIZE)
#define FLASH_MAX_ADDRESS   (FLASH_BASE + FLASH_SIZE - 1)

//--------------------------------------------------------------------
// Funcion declarations
//--------------------------------------------------------------------

void flash_init();
void flash_latency_config(uint32_t ulSystemClock);
void flash_lock();
void flash_unlock();
void flash_page_erase(uint32_t ulAddress);
void flash_page_write(uint32_t ulAddress, uint8_t *pubData, uint32_t ulSize);

#endif
