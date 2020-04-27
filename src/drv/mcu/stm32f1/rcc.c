// SPDX-License-Identifier: MIT

#include "rcc.h"

//--------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------

uint32_t SYS_CLOCK_FREQ;
uint32_t AHB_CLOCK_FREQ;
uint32_t APB1_CLOCK_FREQ;
uint32_t APB2_CLOCK_FREQ;
uint32_t APB1_TIM_CLOCK_FREQ;
uint32_t APB2_TIM_CLOCK_FREQ;
uint32_t ADC_CLOCK_FREQ;

//--------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------

void rcc_init(uint32_t pllMull)
{
    RCC->CR |= RCC_CR_HSEON | RCC_CR_CSSON; // Enable HSE and CSS

    while(!(RCC->CR & RCC_CR_HSERDY)); // Wait until HSE is ready

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1; // AHB Clock = Sys Clock
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; // APB2 Clock = Sys Clock
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 Clock = Sys Clock

    RCC->CFGR &= ~RCC_CFGR_USBPRE; // USB prescaler = PLL clock is divided by 1.5 -> 72 MHz / 1.5 = 48MHz

    RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL)) | (RCC_CFGR_SW_HSE << RCC_CFGR_PLLSRC_Pos) | pllMull; // PLLCLK = HSE * pllMull = 72 MHz

    RCC->CR |= RCC_CR_PLLON; // Enable PLL

    while(!(RCC->CR & RCC_CR_PLLRDY)); // Wait until PLL lock

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL; // PLL as Sys Clock

    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    RCC->CR &= ~RCC_CR_HSION; // Disable HSI

    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8; // ADC clock /8 (min clock)

    RCC->CSR |= RCC_CSR_LSION; // Enable LSI

    while(!(RCC->CSR & RCC_CSR_LSIRDY)); // Wait for LSI to become ready
}
void rcc_update_clocks()
{
    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
        case RCC_CFGR_SWS_HSI:
            SYS_CLOCK_FREQ = HSI_VALUE;
        break;
        case RCC_CFGR_SWS_HSE:
            SYS_CLOCK_FREQ = EXT_OSC_VALUE;
        break;
        case RCC_CFGR_SWS_PLL:
        {
            uint32_t pllmul = ((RCC->CFGR & RCC_CFGR_PLLMULL) >> 18) + 2;
            uint32_t pllsrc = RCC->CFGR & RCC_CFGR_PLLSRC;

            if (pllsrc == 0) // 1 means PLL INPUT = HSI / 2
            {
                SYS_CLOCK_FREQ = (HSI_VALUE >> 1) * pllmul;
            }
            else // 1 means PLL INPUT = HSE (divided by 2 if XTPRE = 1)
            {
                if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) == RCC_CFGR_PLLXTPRE_HSE_DIV2)
                    SYS_CLOCK_FREQ = (EXT_OSC_VALUE >> 1) * pllmul;
                else
                    SYS_CLOCK_FREQ = EXT_OSC_VALUE * pllmul;
            }
            break;
        }
    }

    switch(RCC->CFGR & RCC_CFGR_HPRE)
    {
        case RCC_CFGR_HPRE_DIV1:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ;
        break;
        case RCC_CFGR_HPRE_DIV2:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 1;
        break;
        case RCC_CFGR_HPRE_DIV4:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 2;
        break;
        case RCC_CFGR_HPRE_DIV8:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 3;
        break;
        case RCC_CFGR_HPRE_DIV16:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 4;
        break;
        case RCC_CFGR_HPRE_DIV64:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 6;
        break;
        case RCC_CFGR_HPRE_DIV128:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 7;
        break;
        case RCC_CFGR_HPRE_DIV256:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 8;
        break;
        case RCC_CFGR_HPRE_DIV512:
            AHB_CLOCK_FREQ = SYS_CLOCK_FREQ >> 9;
        break;
    }

    switch(RCC->CFGR & RCC_CFGR_PPRE1)
    {
        case RCC_CFGR_PPRE1_DIV1:
            APB1_CLOCK_FREQ = AHB_CLOCK_FREQ;
        break;
        case RCC_CFGR_PPRE1_DIV2:
            APB1_CLOCK_FREQ = AHB_CLOCK_FREQ >> 1;
        break;
        case RCC_CFGR_PPRE1_DIV4:
            APB1_CLOCK_FREQ = AHB_CLOCK_FREQ >> 2;
        break;
        case RCC_CFGR_PPRE1_DIV8:
            APB1_CLOCK_FREQ = AHB_CLOCK_FREQ >> 3;
        break;
        case RCC_CFGR_PPRE1_DIV16:
            APB1_CLOCK_FREQ = AHB_CLOCK_FREQ >> 4;
        break;
    }

    switch(RCC->CFGR & RCC_CFGR_PPRE2)
    {
        case RCC_CFGR_PPRE2_DIV1:
            APB2_CLOCK_FREQ = AHB_CLOCK_FREQ;
        break;
        case RCC_CFGR_PPRE2_DIV2:
            APB2_CLOCK_FREQ = AHB_CLOCK_FREQ >> 1;
        break;
        case RCC_CFGR_PPRE2_DIV4:
            APB2_CLOCK_FREQ = AHB_CLOCK_FREQ >> 2;
        break;
        case RCC_CFGR_PPRE2_DIV8:
            APB2_CLOCK_FREQ = AHB_CLOCK_FREQ >> 3;
        break;
        case RCC_CFGR_PPRE2_DIV16:
            APB2_CLOCK_FREQ = AHB_CLOCK_FREQ >> 4;
        break;
    }

    if((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1)
        APB1_TIM_CLOCK_FREQ = APB1_CLOCK_FREQ;
    else
        APB1_TIM_CLOCK_FREQ = APB1_CLOCK_FREQ >> 1;

    if((RCC->CFGR & RCC_CFGR_PPRE2) == RCC_CFGR_PPRE2_DIV1)
        APB2_TIM_CLOCK_FREQ = APB2_CLOCK_FREQ;
    else
        APB2_TIM_CLOCK_FREQ = APB2_CLOCK_FREQ >> 1;

    switch(RCC->CFGR & RCC_CFGR_ADCPRE)
    {
        case RCC_CFGR_ADCPRE_DIV2:
            ADC_CLOCK_FREQ = APB2_CLOCK_FREQ >> 1;
        break;
        case RCC_CFGR_ADCPRE_DIV4:
            ADC_CLOCK_FREQ = APB2_CLOCK_FREQ >> 2;
        break;
        case RCC_CFGR_ADCPRE_DIV6:
            ADC_CLOCK_FREQ = APB2_CLOCK_FREQ / 6;
        break;
        case RCC_CFGR_ADCPRE_DIV8:
            ADC_CLOCK_FREQ = APB2_CLOCK_FREQ >> 3;
        break;
    }
}