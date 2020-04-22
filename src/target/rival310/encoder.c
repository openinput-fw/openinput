// SPDX-License-Identifier: MIT

#include "encoder.h"

//--------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------

int8_t encoderDelta = 0;

//--------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------

void encoder_init()
{
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST; // Reset peripheral
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Enable TIM3 peripheral clock

    while(TIM3->CR1 & TIM_CR1_CEN)
        TIM3->CR1 &= ~TIM_CR1_CEN; // Disable timer

    // Make sure the peripheral has the reset values
    TIM3->CR1 = 0x0000;             /*!< TIM control register 1,                      Address offset: 0x00 */
    TIM3->CR2 = 0x0000;             /*!< TIM control register 2,                      Address offset: 0x04 */
    TIM3->SMCR = 0x0000;            /*!< TIM slave Mode Control register,             Address offset: 0x08 */
    TIM3->DIER = 0x0000;            /*!< TIM DMA/interrupt enable register,           Address offset: 0x0C */
    TIM3->SR = 0x0000;              /*!< TIM status register,                         Address offset: 0x10 */
    TIM3->EGR = 0x0000;             /*!< TIM event generation register,               Address offset: 0x14 */
    TIM3->CCMR1 = 0x0000;           /*!< TIM  capture/compare mode register 1,        Address offset: 0x18 */
    TIM3->CCMR2 = 0x0000;           /*!< TIM  capture/compare mode register 2,        Address offset: 0x1C */
    TIM3->CCER = 0x0000;            /*!< TIM capture/compare enable register,         Address offset: 0x20 */
    TIM3->CNT = 0x0000;             /*!< TIM counter register,                        Address offset: 0x24 */
    TIM3->PSC = 0x0000;             /*!< TIM prescaler register,                      Address offset: 0x28 */
    TIM3->ARR = 0x00FF;             /*!< TIM auto-reload register,                    Address offset: 0x2C */
    TIM3->RCR = 0x0000;             /*!< TIM  repetition counter register,            Address offset: 0x30 */
    TIM3->CCR1 = 0x0000;            /*!< TIM capture/compare register 1,              Address offset: 0x34 */
    TIM3->CCR2 = 0x0000;            /*!< TIM capture/compare register 2,              Address offset: 0x38 */
    TIM3->CCR3 = 0x0000;            /*!< TIM capture/compare register 3,              Address offset: 0x3C */
    TIM3->CCR4 = 0x0000;            /*!< TIM capture/compare register 4,              Address offset: 0x40 */
    TIM3->BDTR = 0x0000;            /*!< TIM break and dead-time register,            Address offset: 0x44 */
    TIM3->DCR = 0x0000;             /*!< TIM DMA control register,                    Address offset: 0x48 */
    TIM3->DMAR = 0x0000;            /*!< TIM DMA address for full transfer register,  Address offset: 0x4C */
    TIM3->OR = 0x0000;              /*!< TIM option register,                         Address offset: 0x50 */

    TIM3->PSC = 0;

    //TIM3->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1; // Encoder mode 3 - Counter counts up/down on both TI1FP1 and TI2FP2 edges
    TIM3->SMCR = TIM_SMCR_SMS_0; // Encoder mode 1 - Counter counts up/down on TI2FP1

    TIM3->CCMR1 = TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0 | (0xF << TIM_CCMR1_IC1F_Pos) | (0xF << TIM_CCMR1_IC2F_Pos);  //  TI1FP1 mapped on TI1, TI2FP2 mapped on TI2

    TIM3->CR1 |= TIM_CR1_CEN; // enable timer

    encoderDelta = 0;
}

void encoder_task()
{
    int8_t count = (int8_t)(TIM3->CNT & 0xFF);
    TIM3->CNT = 0;

    encoderDelta += count;
}

int8_t encoder_get_delta()
{
    int8_t delta = encoderDelta;
    encoderDelta = 0;
    return delta;
}