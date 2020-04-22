// SPDX-License-Identifier: MIT

#include "leds.h"

//--------------------------------------------------------------------
// Funcions
//--------------------------------------------------------------------

void leds_init()
{
  RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST; // Reset peripheral
  RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // Enable TIM1 peripheral clock

  while(TIM1->CR1 & TIM_CR1_CEN)
      TIM1->CR1 &= ~TIM_CR1_CEN; // Disable timer1

  RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST; // Reset peripheral
  RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 peripheral clock

  while(TIM2->CR1 & TIM_CR1_CEN)
      TIM2->CR1 &= ~TIM_CR1_CEN; // Disable timer2

  // Make sure the peripheral has the reset values
  TIM1->CR1 = 0x0000;             /*!< TIM control register 1,                      Address offset: 0x00 */
  TIM1->CR2 = 0x0000;             /*!< TIM control register 2,                      Address offset: 0x04 */
  TIM1->SMCR = 0x0000;            /*!< TIM slave Mode Control register,             Address offset: 0x08 */
  TIM1->DIER = 0x0000;            /*!< TIM DMA/interrupt enable register,           Address offset: 0x0C */
  TIM1->SR = 0x0000;              /*!< TIM status register,                         Address offset: 0x10 */
  TIM1->EGR = 0x0000;             /*!< TIM event generation register,               Address offset: 0x14 */
  TIM1->CCMR1 = 0x0000;           /*!< TIM  capture/compare mode register 1,        Address offset: 0x18 */
  TIM1->CCMR2 = 0x0000;           /*!< TIM  capture/compare mode register 2,        Address offset: 0x1C */
  TIM1->CCER = 0x0000;            /*!< TIM capture/compare enable register,         Address offset: 0x20 */
  TIM1->CNT = 0x0000;             /*!< TIM counter register,                        Address offset: 0x24 */
  TIM1->PSC = 0x0000;             /*!< TIM prescaler register,                      Address offset: 0x28 */
  TIM1->ARR = 0xFFFF;             /*!< TIM auto-reload register,                    Address offset: 0x2C */
  TIM1->RCR = 0x0000;             /*!< TIM  repetition counter register,            Address offset: 0x30 */
  TIM1->CCR1 = 0x0000;            /*!< TIM capture/compare register 1,              Address offset: 0x34 */
  TIM1->CCR2 = 0x0000;            /*!< TIM capture/compare register 2,              Address offset: 0x38 */
  TIM1->CCR3 = 0x0000;            /*!< TIM capture/compare register 3,              Address offset: 0x3C */
  TIM1->CCR4 = 0x0000;            /*!< TIM capture/compare register 4,              Address offset: 0x40 */
  TIM1->BDTR = 0x0000;            /*!< TIM break and dead-time register,            Address offset: 0x44 */
  TIM1->DCR = 0x0000;             /*!< TIM DMA control register,                    Address offset: 0x48 */
  TIM1->DMAR = 0x0000;            /*!< TIM DMA address for full transfer register,  Address offset: 0x4C */
  TIM1->OR = 0x0000;              /*!< TIM option register,                         Address offset: 0x50 */

  TIM2->CR1 = 0x0000;             /*!< TIM control register 1,                      Address offset: 0x00 */
  TIM2->CR2 = 0x0000;             /*!< TIM control register 2,                      Address offset: 0x04 */
  TIM2->SMCR = 0x0000;            /*!< TIM slave Mode Control register,             Address offset: 0x08 */
  TIM2->DIER = 0x0000;            /*!< TIM DMA/interrupt enable register,           Address offset: 0x0C */
  TIM2->SR = 0x0000;              /*!< TIM status register,                         Address offset: 0x10 */
  TIM2->EGR = 0x0000;             /*!< TIM event generation register,               Address offset: 0x14 */
  TIM2->CCMR1 = 0x0000;           /*!< TIM  capture/compare mode register 1,        Address offset: 0x18 */
  TIM2->CCMR2 = 0x0000;           /*!< TIM  capture/compare mode register 2,        Address offset: 0x1C */
  TIM2->CCER = 0x0000;            /*!< TIM capture/compare enable register,         Address offset: 0x20 */
  TIM2->CNT = 0x0000;             /*!< TIM counter register,                        Address offset: 0x24 */
  TIM2->PSC = 0x0000;             /*!< TIM prescaler register,                      Address offset: 0x28 */
  TIM2->ARR = 0xFFFF;             /*!< TIM auto-reload register,                    Address offset: 0x2C */
  TIM2->RCR = 0x0000;             /*!< TIM  repetition counter register,            Address offset: 0x30 */
  TIM2->CCR1 = 0x0000;            /*!< TIM capture/compare register 1,              Address offset: 0x34 */
  TIM2->CCR2 = 0x0000;            /*!< TIM capture/compare register 2,              Address offset: 0x38 */
  TIM2->CCR3 = 0x0000;            /*!< TIM capture/compare register 3,              Address offset: 0x3C */
  TIM2->CCR4 = 0x0000;            /*!< TIM capture/compare register 4,              Address offset: 0x40 */
  TIM2->BDTR = 0x0000;            /*!< TIM break and dead-time register,            Address offset: 0x44 */
  TIM2->DCR = 0x0000;             /*!< TIM DMA control register,                    Address offset: 0x48 */
  TIM2->DMAR = 0x0000;            /*!< TIM DMA address for full transfer register,  Address offset: 0x4C */
  TIM2->OR = 0x0000;              /*!< TIM option register,                         Address offset: 0x50 */

  // timer configuration
  TIM1->ARR = (1 << 8) - 1; // 8-bit PWM
  TIM1->PSC = 6; // frequency = [APB1_TIM_CLOCK_FREQ / TIM1->PSC + 1 / TIM1->ARR] = [36MHz / 7 / 255] =  20.168KHz
  TIM1->EGR |= TIM_EGR_UG; // Update immediatly

  // capture compare configuration
  TIM1->CCMR1 = (6 << 4) | (6 << 12); // PWM Mode 1 (CH1 & CH2)
  TIM1->CCMR2 = (6 << 4); // PWM Mode 1 (CH3)
  TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E; // Enable CH1, CH2, CH3, active high

  // capture compare values
  //TIM1->CCR1 = 0; // capture compare ch1, sets green duty cycle (0-255)
  //TIM1->CCR2 = 0; // capture compare ch2, sets red duty cycle (0-255)
  //TIM1->CCR3 = 0; // capture compare ch3, sets blue duty cycle (0-255)

  // timer configuration
  TIM2->ARR = (1 << 8) - 1; // 8-bit PWM
  TIM2->PSC = 6; // frequency = [APB1_TIM_CLOCK_FREQ / TIM1->PSC + 1 / TIM1->ARR] = [36MHz / 7 / 255] =  20.168KHz
  TIM2->EGR |= TIM_EGR_UG; // Update immediatly

  // capture compare configuration
  TIM2->CCMR1 = (6 << 4) | (6 << 12); // PWM Mode 1 (CH1 & CH2)
  TIM2->CCMR2 = (6 << 4); // PWM Mode 1 (CH3)
  TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E; // Enable CH1, CH2, CH3, active high

  // capture compare values
  //TIM2->CCR1 = 0; // capture compare ch1, sets green duty cycle (0-255)
  //TIM2->CCR2 = 0; // capture compare ch2, sets red duty cycle (0-255)
  //TIM2->CCR3 = 0; // capture compare ch3, sets blue duty cycle (0-255)

  TIM1->BDTR = TIM_BDTR_MOE; // Main timer1 output enable
  TIM2->BDTR = TIM_BDTR_MOE; // Main timer2 output enable

  TIM1->CR1 |= TIM_CR1_CEN; // enable timer1
  TIM2->CR1 |= TIM_CR1_CEN; // enable timer2
}

void led_logo_write(uint8_t r, uint8_t g, uint8_t b)
{
  TIM1->CCR2 = r;
  TIM1->CCR1 = g;
  TIM1->CCR3 = b;
}
void led_wheel_write(uint8_t r, uint8_t g, uint8_t b)
{
  TIM2->CCR2 = r;
  TIM2->CCR1 = g;
  TIM2->CCR3 = b;
}