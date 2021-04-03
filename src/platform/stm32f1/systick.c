/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/atomic.h"
#include "platform/stm32f1/rcc.h"
#include "platform/stm32f1/systick.h"
#include "util/data.h"

static volatile u64 system_tick = 0;

static u32 sys_clock_freq;

void _systick_isr()
{
	system_tick++;
}

void systick_init()
{
	struct rcc_clock_tree_t clock_tree = rcc_get_clock_tree();

	sys_clock_freq = clock_tree.sys_clock_freq;

	/* ms tick initialization */
	SysTick->LOAD = (sys_clock_freq / 8 / 1000) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	SCB->SHP[11] = 15 << (8 - __NVIC_PRIO_BITS); /* Set priority 7,1 (min) */

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
		uint64_t start_tick = system_tick;

		while ((system_tick - start_tick) < ticks) continue;
	}
}

void delay_us(u32 ticks)
{
	u32 start_tick = DWT->CYCCNT;

	ticks *= (sys_clock_freq / 1000000);

	while (DWT->CYCCNT - start_tick < ticks) continue;
}
