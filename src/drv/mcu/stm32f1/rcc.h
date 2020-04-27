// SPDX-License-Identifier: MIT

#ifndef __RCC_H__
#define __RCC_H__

#include "config.h"
#include <stm32f1xx.h>

//--------------------------------------------------------------------
// Definitions
//--------------------------------------------------------------------

#ifndef HSI_VALUE
#define HSI_VALUE   8000000UL
#endif

//--------------------------------------------------------------------
// Global system clock variables
//--------------------------------------------------------------------

extern uint32_t SYS_CLOCK_FREQ;
extern uint32_t AHB_CLOCK_FREQ;
extern uint32_t APB1_CLOCK_FREQ;
extern uint32_t APB2_CLOCK_FREQ;
extern uint32_t APB1_TIM_CLOCK_FREQ;
extern uint32_t APB2_TIM_CLOCK_FREQ;
extern uint32_t ADC_CLOCK_FREQ;

//--------------------------------------------------------------------
// Funcion declarations
//--------------------------------------------------------------------

void rcc_init(uint32_t pllMull);
void rcc_update_clocks();

#endif
