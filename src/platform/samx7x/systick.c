/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/samx7x/atomic.h"
#include "platform/samx7x/pmc.h"
#include "platform/samx7x/systick.h"
#include "util/data.h"

static volatile u64 system_tick = 0;

static u32 systick_clock_freq;

void _systick_isr()
{
	system_tick++;
}

void systick_init()
{
	struct pmc_clock_tree_t clock_tree = pmc_get_clock_tree();

	systick_clock_freq = clock_tree.fclk_freq;

	/* ms tick initialization */
	SysTick->LOAD = (systick_clock_freq / 1000) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;

	SCB->SHPR[11] = 7 << (8 - __NVIC_PRIO_BITS); // Set priority 3,1 (min)

	/* delay us initialization */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; /* Enable TRC */
	DWT->LAR = 0xC5ACCE55;
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

	ticks *= (systick_clock_freq / 1000000);

	while (DWT->CYCCNT - start_tick < ticks) continue;
}
