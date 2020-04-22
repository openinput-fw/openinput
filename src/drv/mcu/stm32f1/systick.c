// SPDX-License-Identifier: MIT

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
