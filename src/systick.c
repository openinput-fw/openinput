#include "systick.h"

volatile uint64_t systemTick = 0;

void sys_tick_handler()
{
	systemTick++;
}
void systick_init()
{
	/* 72MHz / 8 => 9000000 counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

	/* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(8999);

	systick_interrupt_enable();

	/* Start counting. */
	systick_counter_enable();
}
void delay_ms(uint64_t ticks)
{
	// TODO: make sure interrupts are enabled otherwise we stuck
    //NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
    //{
        uint64_t startTick = systemTick;

        while((systemTick - startTick) < ticks);
    //}
}
