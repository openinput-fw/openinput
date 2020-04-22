// SPDX-License-Identifier: MIT

#include "gpio.h"

//--------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------

// TODO: Complete rework

void gpio_init(gpio_init_t* gpiocfg)
{
    RCC->APB2RSTR |= RCC_APB2RSTR_IOPARST; // Reset peripheral
    RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPARST;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Enable GPIOA peripheral clock
    GPIOA->CRL = _IO_MODE_CFG(0,  gpiocfg->port[0].MODE_CFG[0]) |
                 _IO_MODE_CFG(1,  gpiocfg->port[0].MODE_CFG[1]) |
                 _IO_MODE_CFG(2,  gpiocfg->port[0].MODE_CFG[2]) |
                 _IO_MODE_CFG(3,  gpiocfg->port[0].MODE_CFG[3]) |
                 _IO_MODE_CFG(4,  gpiocfg->port[0].MODE_CFG[4]) |
                 _IO_MODE_CFG(5,  gpiocfg->port[0].MODE_CFG[5]) |
                 _IO_MODE_CFG(6,  gpiocfg->port[0].MODE_CFG[6]) |
                 _IO_MODE_CFG(7,  gpiocfg->port[0].MODE_CFG[7]);
    GPIOA->CRH = _IO_MODE_CFG(8,  gpiocfg->port[0].MODE_CFG[8]) |
                 _IO_MODE_CFG(9,  gpiocfg->port[0].MODE_CFG[9]) |
                 _IO_MODE_CFG(10, gpiocfg->port[0].MODE_CFG[10]) |
                 _IO_MODE_CFG(11, gpiocfg->port[0].MODE_CFG[11]) |
                 _IO_MODE_CFG(12, gpiocfg->port[0].MODE_CFG[12]) |
                 _IO_MODE_CFG(13, gpiocfg->port[0].MODE_CFG[13]) |
                 _IO_MODE_CFG(14, gpiocfg->port[0].MODE_CFG[14]) |
                 _IO_MODE_CFG(15, gpiocfg->port[0].MODE_CFG[15]) ;
    GPIOA->ODR = gpiocfg->port[0].ODR;

    RCC->APB2RSTR |= RCC_APB2RSTR_IOPBRST; // Reset peripheral
    RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPBRST;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Enable GPIOB peripheral clock
    GPIOB->CRL = _IO_MODE_CFG(0,  gpiocfg->port[1].MODE_CFG[0]) |
                 _IO_MODE_CFG(1,  gpiocfg->port[1].MODE_CFG[1]) |
                 _IO_MODE_CFG(2,  gpiocfg->port[1].MODE_CFG[2]) |
                 _IO_MODE_CFG(3,  gpiocfg->port[1].MODE_CFG[3]) |
                 _IO_MODE_CFG(4,  gpiocfg->port[1].MODE_CFG[4]) |
                 _IO_MODE_CFG(5,  gpiocfg->port[1].MODE_CFG[5]) |
                 _IO_MODE_CFG(6,  gpiocfg->port[1].MODE_CFG[6]) |
                 _IO_MODE_CFG(7,  gpiocfg->port[1].MODE_CFG[7]);
    GPIOB->CRH = _IO_MODE_CFG(8,  gpiocfg->port[1].MODE_CFG[8]) |
                 _IO_MODE_CFG(9,  gpiocfg->port[1].MODE_CFG[9]) |
                 _IO_MODE_CFG(10, gpiocfg->port[1].MODE_CFG[10]) |
                 _IO_MODE_CFG(11, gpiocfg->port[1].MODE_CFG[11]) |
                 _IO_MODE_CFG(12, gpiocfg->port[1].MODE_CFG[12]) |
                 _IO_MODE_CFG(13, gpiocfg->port[1].MODE_CFG[13]) |
                 _IO_MODE_CFG(14, gpiocfg->port[1].MODE_CFG[14]) |
                 _IO_MODE_CFG(15, gpiocfg->port[1].MODE_CFG[15]) ;
    GPIOB->ODR = gpiocfg->port[1].ODR;

    RCC->APB2RSTR |= RCC_APB2RSTR_IOPCRST; // Reset peripheral
    RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPCRST;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Enable GPIOC peripheral clock
    GPIOC->CRL = _IO_MODE_CFG(0,  gpiocfg->port[2].MODE_CFG[0]) |
                 _IO_MODE_CFG(1,  gpiocfg->port[2].MODE_CFG[1]) |
                 _IO_MODE_CFG(2,  gpiocfg->port[2].MODE_CFG[2]) |
                 _IO_MODE_CFG(3,  gpiocfg->port[2].MODE_CFG[3]) |
                 _IO_MODE_CFG(4,  gpiocfg->port[2].MODE_CFG[4]) |
                 _IO_MODE_CFG(5,  gpiocfg->port[2].MODE_CFG[5]) |
                 _IO_MODE_CFG(6,  gpiocfg->port[2].MODE_CFG[6]) |
                 _IO_MODE_CFG(7,  gpiocfg->port[2].MODE_CFG[7]);
    GPIOC->CRH = _IO_MODE_CFG(8,  gpiocfg->port[2].MODE_CFG[8]) |
                 _IO_MODE_CFG(9,  gpiocfg->port[2].MODE_CFG[9]) |
                 _IO_MODE_CFG(10, gpiocfg->port[2].MODE_CFG[10]) |
                 _IO_MODE_CFG(11, gpiocfg->port[2].MODE_CFG[11]) |
                 _IO_MODE_CFG(12, gpiocfg->port[2].MODE_CFG[12]) |
                 _IO_MODE_CFG(13, gpiocfg->port[2].MODE_CFG[13]) |
                 _IO_MODE_CFG(14, gpiocfg->port[2].MODE_CFG[14]) |
                 _IO_MODE_CFG(15, gpiocfg->port[2].MODE_CFG[15]) ;
    GPIOC->ODR = gpiocfg->port[2].ODR;

    RCC->APB2RSTR |= RCC_APB2RSTR_IOPDRST; // Reset peripheral
    RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPDRST;
    RCC->APB2ENR |= RCC_APB2ENR_IOPDEN; // Enable GPIOD peripheral clock
    GPIOD->CRL = _IO_MODE_CFG(0,  gpiocfg->port[3].MODE_CFG[0]) |
                 _IO_MODE_CFG(1,  gpiocfg->port[3].MODE_CFG[1]) |
                 _IO_MODE_CFG(2,  gpiocfg->port[3].MODE_CFG[2]) |
                 _IO_MODE_CFG(3,  gpiocfg->port[3].MODE_CFG[3]) |
                 _IO_MODE_CFG(4,  gpiocfg->port[3].MODE_CFG[4]) |
                 _IO_MODE_CFG(5,  gpiocfg->port[3].MODE_CFG[5]) |
                 _IO_MODE_CFG(6,  gpiocfg->port[3].MODE_CFG[6]) |
                 _IO_MODE_CFG(7,  gpiocfg->port[3].MODE_CFG[7]);
    GPIOD->CRH = _IO_MODE_CFG(8,  gpiocfg->port[3].MODE_CFG[8]) |
                 _IO_MODE_CFG(9,  gpiocfg->port[3].MODE_CFG[9]) |
                 _IO_MODE_CFG(10, gpiocfg->port[3].MODE_CFG[10]) |
                 _IO_MODE_CFG(11, gpiocfg->port[3].MODE_CFG[11]) |
                 _IO_MODE_CFG(12, gpiocfg->port[3].MODE_CFG[12]) |
                 _IO_MODE_CFG(13, gpiocfg->port[3].MODE_CFG[13]) |
                 _IO_MODE_CFG(14, gpiocfg->port[3].MODE_CFG[14]) |
                 _IO_MODE_CFG(15, gpiocfg->port[3].MODE_CFG[15]) ;
    GPIOD->ODR = gpiocfg->port[3].ODR;
}