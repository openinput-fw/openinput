#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

extern volatile uint64_t systemTick;

void systick_init();
void delay_ms(uint64_t ticks);

#endif // __SYSTICK_H__