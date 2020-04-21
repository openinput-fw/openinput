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

#include "systick.h"

//--------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------

volatile uint64_t systemTick = 0;

//--------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------

void _systick_isr()
{
    systemTick++;
}
void systick_init()
{
    // delay ms initialization
    SysTick->LOAD = (SYS_CLOCK_FREQ / 8 / 1000) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

    SCB->SHP[11] = 15 << (8 - __NVIC_PRIO_BITS); // Set priority 7,1 (min)

    // delay us initialization
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable TRC
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;    // Enable  clock cycle counter
}
void delay_ms(uint32_t ticks)
{
    NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
    {
        uint64_t startTick = systemTick;

        while((systemTick - startTick) < ticks);
    }
}
void delay_us(uint32_t ticks)
{
    uint32_t startTick = DWT->CYCCNT;
    ticks *= (SYS_CLOCK_FREQ / 1000000);

    while(DWT->CYCCNT - startTick < ticks);
}
