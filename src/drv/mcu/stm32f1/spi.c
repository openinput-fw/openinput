/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 openinput (https://github.com/openinput-fw)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "spi.h"

//--------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------

void spi_init(uint8_t ubMode, uint8_t ubClockDiv, uint8_t ubBitMode)
{
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST; // Reset peripheral
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI peripheral clock

    SPI1->I2SCFGR = 0x0000;

    SPI1->CR2 = 0x00000000;
    SPI1->CR1 = (uint32_t)(ubMode | ubClockDiv | ubBitMode) | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE;
}
uint8_t spi_transfer_byte(const uint8_t ubData)
{
    while(!(SPI1->SR & SPI_SR_TXE));

    SPI1->DR = ubData;

    while(!(SPI1->SR & SPI_SR_RXNE));

    return (uint8_t)SPI1->DR;
}
void spi_transfer(const uint8_t* pubSrc, uint32_t ulSize, uint8_t* pubDst)
{
    if(pubSrc)
    {
        while(ulSize--)
        {
            if(pubDst)
                *(pubDst++) = spi_transfer_byte(*(pubSrc++));
            else
                spi_transfer_byte(*(pubSrc++));
        }
    }
    else if(pubDst)
    {
        while(ulSize--)
            *(pubDst++) = spi_transfer_byte(0x00);
    }
}