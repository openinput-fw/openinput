/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/driver/led_pwm.h"
#include "platform/stm32f1/rcc.h"

struct led_pwm_data_t
led_pwm_init(enum timer_no timer_no, enum cc_no red_ch, enum cc_no green_ch, enum cc_no blue_ch, u16 frequency)
{
	TIM_TypeDef *timer;

	/* get clock tree */
	struct rcc_clock_tree_t clock_tree = rcc_get_clock_tree();

	u32 tim_clock_freq;

	switch (timer_no) {
		case timer1:
			RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST; /* Reset peripheral */
			RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; /* Enable TIM1 peripheral clock */
			timer = TIM1;
			tim_clock_freq = clock_tree.apb2_tim_clock_freq;
			break;

		case timer2:
			RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST; /* Reset peripheral */
			RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; /* Enable TIM2 peripheral clock */
			timer = TIM2;
			tim_clock_freq = clock_tree.apb1_tim_clock_freq;
			break;
	}

	u32 ccmr1 = 0, ccmr2 = 0, ccer = 0;

	if (red_ch == cc1 || green_ch == cc1 || blue_ch == cc1) {
		ccmr1 |= (6 << 4); /* PWM Mode cc1 */
		ccer |= TIM_CCER_CC1E;
	}
	if (red_ch == cc2 || green_ch == cc2 || blue_ch == cc2) {
		ccmr1 |= (6 << 12); /* PWM Mode cc2 */
		ccer |= TIM_CCER_CC2E;
	}
	if (red_ch == cc3 || green_ch == cc3 || blue_ch == cc3) {
		ccmr2 |= (6 << 4); /* PWM Mode cc3 */
		ccer |= TIM_CCER_CC3E;
	}
	if (red_ch == cc4 || green_ch == cc4 || blue_ch == cc4) {
		ccmr2 |= (6 << 12); /* PWM Mode cc4 */
		ccer |= TIM_CCER_CC4E;
	}

	struct led_pwm_data_t led_data;

	switch (red_ch) {
		case cc1:
			led_data.red_reg = (u32 *) &timer->CCR1;
			break;

		case cc2:
			led_data.red_reg = (u32 *) &timer->CCR2;
			break;

		case cc3:
			led_data.red_reg = (u32 *) &timer->CCR3;
			break;

		case cc4:
			led_data.red_reg = (u32 *) &timer->CCR4;
			break;
	}
	switch (green_ch) {
		case cc1:
			led_data.green_reg = (u32 *) &timer->CCR1;
			break;

		case cc2:
			led_data.green_reg = (u32 *) &timer->CCR2;
			break;

		case cc3:
			led_data.green_reg = (u32 *) &timer->CCR3;
			break;

		case cc4:
			led_data.green_reg = (u32 *) &timer->CCR4;
			break;
	}
	switch (blue_ch) {
		case cc1:
			led_data.blue_reg = (u32 *) &timer->CCR1;
			break;

		case cc2:
			led_data.blue_reg = (u32 *) &timer->CCR2;
			break;

		case cc3:
			led_data.blue_reg = (u32 *) &timer->CCR3;
			break;

		case cc4:
			led_data.blue_reg = (u32 *) &timer->CCR4;
			break;
	}

	/* Disable timer1 */
	while (timer->CR1 & TIM_CR1_CEN) timer->CR1 &= ~TIM_CR1_CEN;

	/* timer configuration */
	timer->ARR = (1 << 8) - 1; /* 8-bit PWM */
	timer->PSC = (tim_clock_freq / (255 * frequency)) - 1;
	timer->EGR |= TIM_EGR_UG; /* Update immediatly */

	/* capture compare configuration */
	timer->CCMR1 = ccmr1;
	timer->CCMR2 = ccmr2;
	timer->CCER = ccer; /* Enable active channels active high */

	timer->BDTR = TIM_BDTR_MOE; /* Main timer output enable */

	timer->CR1 |= TIM_CR1_CEN; /* enable timer */

	return led_data;
}

void led_pwm_set_color(struct led_pwm_data_t led_data, u8 red, u8 green, u8 blue)
{
	*led_data.red_reg = red;
	*led_data.green_reg = green;
	*led_data.blue_reg = blue;
}
