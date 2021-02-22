/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stm32f1xx.h>

#include "platform/stm32f1/rcc.h"
#include "util/data.h"

#define HSI_VALUE 8000000UL

static struct rcc_clock_tree_t rcc_clock_tree;

/*
 * Setup the clock using the HSE (external clock) and set SYSCLK to 72MHz.
 *
 * We select the HSE as clock source and pass it trough the PLL, which then goes
 * to the USB interface and the system clock.
 *
 * HSE (freq HW defined) ---PLLXTPRE---> ---PLLSRC---> PULLMUL (72MHz)
 *
 * The HSE frequency value is defined by the hardware and is passed as an
 * argument to this function.
 * We pass the HSE directly through PLLXTPRE, achieving a /1 division factor,
 * then it goes to PLLMUL where it gets multiplied by a certain value,
 * always resulting in a 72MHz output (eg. HSE=8MHz, PULLMUL=x9, 8*9=72). PLLMUL
 * is calculated based on the HSE value.
 *
 * After passing through the PLL, the clock goes to the system clock, and also
 * goes through a prescaler which is then sets the USB frequency.
 *
 * PULLMUL (72MHz) ---SW---> SYSCLOCK (72MHz)
 * PULLMUL (72MHz) ---USBPRE(/1.5)---> USB (48MHz)
 *
 * Then we set the AHB prescaler to /1, APB1 to /2 (as the max is 32MHz), APB2
 * to /1, and ADC to /6 (as the max is 14MHz).
 *
 * SYSCLK (72MHz) ---AHBPRE(/1)--> 72MHz
 *
 * ---AHBPRE(/1)--> 72MHz ---APB1PRE(/2)--> 36MHz
 * ---AHBPRE(/1)--> 72MHz ---APB2PRE(/2)--> 36MHz ---ADCPRE(/8)--> 4.5MHz
 *
 * @param clock		external clock value
 */
void rcc_init(enum stm32f1_external_clock_value clock)
{
	u32 pll_multiplier = 0;

	switch (clock) {
		case STM32F1_CLOCK_8MHZ:
			pll_multiplier = RCC_CFGR_PLLMULL9; /* x9 */
			break;

		case STM32F1_CLOCK_12MHZ:
			pll_multiplier = RCC_CFGR_PLLMULL6; /* x6 */
			break;

		default:
			break;
	}

	RCC->CR |= RCC_CR_HSEON | RCC_CR_CSSON; /* enable HSE and CSS */
	while (!(RCC->CR & RCC_CR_HSERDY)) continue; /* wait for HSE to be ready */

	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE; /* don't divide in PLLXTPRE */

	RCC->CFGR |= RCC_CFGR_PLLSRC; /* set PLLSRC to HSE */
	REG_SET(RCC->CFGR, RCC_CFGR_PLLMULL, pll_multiplier); /* set PLL multiplier */

	RCC->CFGR &= ~RCC_CFGR_USBPRE; /* set USB prescaler to /1.5 */

	/* set system clock switch to receive from the PLL */
	REG_SET(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
	/* wait for the system clock source to be the PLL */
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) continue;

	REG_SET(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); /* set AHB prescaler to /1 */
	REG_SET(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2); /* set APB1 prescaler to /2 */
	REG_SET(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE1_DIV2); /* set APB2 prescaler to /2 */
	REG_SET(RCC->CFGR, RCC_CFGR_ADCPRE, RCC_CFGR_ADCPRE_DIV8); /* set ADC prescaler to /8 */

	RCC->CR &= ~RCC_CR_HSION; /* disable HSI */

	RCC->CSR |= RCC_CSR_LSION; /* enable LSI */
	while (!(RCC->CSR & RCC_CSR_LSIRDY)) continue; /* wait for LSI to be ready */
}

void rcc_update_clock_tree()
{
	/* Determine system clock */
	switch (RCC->CFGR & RCC_CFGR_SWS) {
		case RCC_CFGR_SWS_HSI:
			rcc_clock_tree.sys_clock_freq = HSI_VALUE;
			break;
		case RCC_CFGR_SWS_HSE:
			rcc_clock_tree.sys_clock_freq = EXTERNAL_CLOCK_VALUE;
			break;
		case RCC_CFGR_SWS_PLL: {
			u32 pllmul = ((RCC->CFGR & RCC_CFGR_PLLMULL) >> 18) + 2;
			u32 pllsrc = RCC->CFGR & RCC_CFGR_PLLSRC;

			if (pllsrc == 0) /* 0 means PLL INPUT = HSI / 2 */
			{
				rcc_clock_tree.sys_clock_freq = (HSI_VALUE >> 1) * pllmul;
			} else /* 1 means PLL INPUT = HSE, HSE divided by 2 if XTPRE = 1 */
			{
				if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) == RCC_CFGR_PLLXTPRE_HSE_DIV2)
					rcc_clock_tree.sys_clock_freq = (EXTERNAL_CLOCK_VALUE >> 1) * pllmul;
				else
					rcc_clock_tree.sys_clock_freq = EXTERNAL_CLOCK_VALUE * pllmul;
			}
			break;
		}
	}

	/* Determine AHB clock */
	switch (RCC->CFGR & RCC_CFGR_HPRE) {
		case RCC_CFGR_HPRE_DIV1:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq;
			break;
		case RCC_CFGR_HPRE_DIV2:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 1;
			break;
		case RCC_CFGR_HPRE_DIV4:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 2;
			break;
		case RCC_CFGR_HPRE_DIV8:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 3;
			break;
		case RCC_CFGR_HPRE_DIV16:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 4;
			break;
		case RCC_CFGR_HPRE_DIV64:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 6;
			break;
		case RCC_CFGR_HPRE_DIV128:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 7;
			break;
		case RCC_CFGR_HPRE_DIV256:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 8;
			break;
		case RCC_CFGR_HPRE_DIV512:
			rcc_clock_tree.ahb_clock_freq = rcc_clock_tree.sys_clock_freq >> 9;
			break;
	}

	/* Determine APB1 clock */
	switch (RCC->CFGR & RCC_CFGR_PPRE1) {
		case RCC_CFGR_PPRE1_DIV1:
			rcc_clock_tree.apb1_clock_freq = rcc_clock_tree.ahb_clock_freq;
			break;
		case RCC_CFGR_PPRE1_DIV2:
			rcc_clock_tree.apb1_clock_freq = rcc_clock_tree.ahb_clock_freq >> 1;
			break;
		case RCC_CFGR_PPRE1_DIV4:
			rcc_clock_tree.apb1_clock_freq = rcc_clock_tree.ahb_clock_freq >> 2;
			break;
		case RCC_CFGR_PPRE1_DIV8:
			rcc_clock_tree.apb1_clock_freq = rcc_clock_tree.ahb_clock_freq >> 3;
			break;
		case RCC_CFGR_PPRE1_DIV16:
			rcc_clock_tree.apb1_clock_freq = rcc_clock_tree.ahb_clock_freq >> 4;
			break;
	}

	/* Determine APB2 clock */
	switch (RCC->CFGR & RCC_CFGR_PPRE2) {
		case RCC_CFGR_PPRE2_DIV1:
			rcc_clock_tree.apb2_clock_freq = rcc_clock_tree.ahb_clock_freq;
			break;
		case RCC_CFGR_PPRE2_DIV2:
			rcc_clock_tree.apb2_clock_freq = rcc_clock_tree.ahb_clock_freq >> 1;
			break;
		case RCC_CFGR_PPRE2_DIV4:
			rcc_clock_tree.apb2_clock_freq = rcc_clock_tree.ahb_clock_freq >> 2;
			break;
		case RCC_CFGR_PPRE2_DIV8:
			rcc_clock_tree.apb2_clock_freq = rcc_clock_tree.ahb_clock_freq >> 3;
			break;
		case RCC_CFGR_PPRE2_DIV16:
			rcc_clock_tree.apb2_clock_freq = rcc_clock_tree.ahb_clock_freq >> 4;
			break;
	}

	/* Determine APB1 TIM clock */
	if ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1)
		rcc_clock_tree.apb1_tim_clock_freq = rcc_clock_tree.apb1_clock_freq;
	else
		rcc_clock_tree.apb1_tim_clock_freq = rcc_clock_tree.apb1_clock_freq >> 1;

	/* Determine APB2 TIM clock */
	if ((RCC->CFGR & RCC_CFGR_PPRE2) == RCC_CFGR_PPRE2_DIV1)
		rcc_clock_tree.apb2_tim_clock_freq = rcc_clock_tree.apb2_clock_freq;
	else
		rcc_clock_tree.apb2_tim_clock_freq = rcc_clock_tree.apb2_clock_freq >> 1;

	/* Determine ADC clock */
	switch (RCC->CFGR & RCC_CFGR_ADCPRE) {
		case RCC_CFGR_ADCPRE_DIV2:
			rcc_clock_tree.adc_clock_freq = rcc_clock_tree.apb2_clock_freq >> 1;
			break;
		case RCC_CFGR_ADCPRE_DIV4:
			rcc_clock_tree.adc_clock_freq = rcc_clock_tree.apb2_clock_freq >> 2;
			break;
		case RCC_CFGR_ADCPRE_DIV6:
			rcc_clock_tree.adc_clock_freq = rcc_clock_tree.apb2_clock_freq / 6;
			break;
		case RCC_CFGR_ADCPRE_DIV8:
			rcc_clock_tree.adc_clock_freq = rcc_clock_tree.apb2_clock_freq >> 3;
			break;
	}
}

struct rcc_clock_tree_t rcc_get_clock_tree()
{
	return rcc_clock_tree;
}
