// SPDX-License-Identifier: MIT

#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "config.h"
#include <stm32f1xx.h>
#include "atomic.h"
#include "rcc.h"

//--------------------------------------------------------------------
// Global system tick
//--------------------------------------------------------------------

extern volatile uint64_t systemTick;

//--------------------------------------------------------------------
// Function declaration
//--------------------------------------------------------------------

void systick_init();
void delay_ms(uint32_t ticks);
void delay_us(uint32_t ticks);

#endif
