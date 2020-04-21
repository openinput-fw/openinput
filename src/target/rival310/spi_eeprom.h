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
 * This file is part of the TinyUSB stack.
 */

#ifndef __SPI_EEPROM_H__
#define __SPI_EEPROM_H__

#include <stm32f1xx.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "systick.h"
#include "atomic.h"
#include "spi.h"

//--------------------------------------------------------------------
// Definitions
//--------------------------------------------------------------------

// IO config
#define SPI_EEPROM_SELECT()      GPIOA->BSRR = BIT(4)
#define SPI_EEPROM_UNSELECT()    GPIOA->BSRR = (BIT(4) << 16)

// Configuration commands
#define SPI_EEPROM_CMD_NOP                   0x00
#define SPI_EEPROM_CMD_READ_STATUS           0x05
#define SPI_EEPROM_CMD_WRITE_STATUS          0x01

// Read commands
#define SPI_EEPROM_CMD_READ                  0x03

// Write commands
#define SPI_EEPROM_CMD_WRITE_ENABLE          0x06
#define SPI_EEPROM_CMD_WRITE_DISABLE         0x04
#define SPI_EEPROM_CMD_WRITE                 0x02

// Status bits
#define SPI_EEPROM_STATUS_WIP    0x01
#define SPI_EEPROM_STATUS_WEL    0x02
#define SPI_EEPROM_STATUS_BP0    0x04
#define SPI_EEPROM_STATUS_BP1    0x08
#define SPI_EEPROM_STATUS_SRWD   0x80

#define SPI_EEPROM_SIZE              ((uint32_t)0x010000) // 64 KB (512 Kbit)
#define SPI_EEPROM_PAGE_SIZE         ((uint32_t)0x000080) // 128 B

#define SPI_EEPROM_PAGE(i)           (SPI_EEPROM_PAGE_SIZE * (i))

#define SPI_EEPROM_PAGE_MASK         (SPI_EEPROM_MAX_ADDRESS & ~(SPI_EEPROM_PAGE_SIZE - 1))

#define SPI_EEPROM_MAX_ADDRESS       (SPI_EEPROM_SIZE - 1)

//--------------------------------------------------------------------
// Function declarations
//--------------------------------------------------------------------

void spi_eeprom_read(uint16_t ulAddress, uint8_t *pubDst, uint32_t ulCount);
void spi_eeprom_write(uint16_t ulAddress, const uint8_t *pubSrc, uint32_t ulCount);
inline uint8_t spi_eeprom_read_byte(uint16_t ulAddress)
{
    uint8_t ubData = 0xFF;

    spi_eeprom_read(ulAddress, &ubData, 1);

    return ubData;
}
inline void spi_eeprom_write_byte(uint16_t ulAddress, uint8_t ubData)
{
    spi_eeprom_write(ulAddress, &ubData, 1);
}

void spi_eeprom_busy_wait();
void spi_eeprom_write_enable();
void spi_eeprom_write_disable();
uint8_t spi_eeprom_read_status();

#endif // __SPI_EEPROM_H__