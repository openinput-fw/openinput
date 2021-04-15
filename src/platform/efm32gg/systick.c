/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <em_device.h>

#include "platform/efm32gg/atomic.h"
#include "platform/efm32gg/cmu.h"
#include "platform/efm32gg/systick.h"
#include "util/data.h"

static volatile u64 system_tick = 0;

static u32 sys_clock_freq;

void _systick_isr()
{
	system_tick++;
}

void systick_init()
{
	struct cmu_clock_tree_t clock_tree = cmu_get_clock_tree();

	sys_clock_freq = clock_tree.hfcore_freq;

	/* ms tick initialization */
	SysTick->LOAD = (sys_clock_freq / 1000) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;

	SCB->SHP[11] = 7 << (8 - __NVIC_PRIO_BITS); // Set priority 3,1 (min)

	/* delay us initialization */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; /* Enable TRC */
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; /* Enable  clock cycle counter */
}

u64 systick_get_ticks()
{
	return system_tick;
}

void delay_ms(u32 ticks)
{
	NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
	{
		u64 start_tick = system_tick;

		while ((system_tick - start_tick) < ticks) continue;
	}
}

void delay_us(u32 ticks)
{
	u32 start_tick = DWT->CYCCNT;

	ticks *= (sys_clock_freq / 1000000);

	while (DWT->CYCCNT - start_tick < ticks) continue;
}
