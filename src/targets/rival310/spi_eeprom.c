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

#include "spi_eeprom.h"

//--------------------------------------------------------------------
// Funcions
//--------------------------------------------------------------------

void spi_eeprom_read(uint16_t ulAddress, uint8_t *pubDst, uint32_t ulCount)
{
    if(!ulCount)
        return;

    SPI_EEPROM_UNSELECT();

    spi_eeprom_busy_wait();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SPI_EEPROM_SELECT();

        spi_transfer_byte(SPI_EEPROM_CMD_READ);
        spi_transfer_byte((ulAddress >> 8) & 0xFF);
        spi_transfer_byte(ulAddress & 0xFF);

        spi_read(pubDst, ulCount);

        SPI_EEPROM_UNSELECT();
    }
}
void spi_eeprom_write(uint16_t ulAddress, const uint8_t *pubSrc, uint32_t ulCount)
{
    if(!ulCount)
        return;

    SPI_EEPROM_UNSELECT();

    spi_eeprom_busy_wait();
    spi_eeprom_write_enable();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SPI_EEPROM_SELECT();

        spi_transfer_byte(SPI_EEPROM_CMD_WRITE);
        spi_transfer_byte((ulAddress >> 8) & 0xFF);
        spi_transfer_byte(ulAddress & 0xFF);

        spi_write(pubSrc, ulCount);

        SPI_EEPROM_UNSELECT();
    }
}

void spi_eeprom_busy_wait()
{
    while(spi_eeprom_read_status() & SPI_EEPROM_STATUS_WIP);
//        delay_ms(1);
}
void spi_eeprom_write_enable()
{
    SPI_EEPROM_UNSELECT();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SPI_EEPROM_SELECT();

        spi_transfer_byte(SPI_EEPROM_CMD_WRITE_ENABLE);

        SPI_EEPROM_UNSELECT();
    }
}
void spi_eeprom_write_disable()
{
    SPI_EEPROM_UNSELECT();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SPI_EEPROM_SELECT();

        spi_transfer_byte(SPI_EEPROM_CMD_WRITE_DISABLE);

        SPI_EEPROM_UNSELECT();
    }
}
uint8_t spi_eeprom_read_status()
{
    uint8_t ubBuf[] = {SPI_EEPROM_CMD_READ_STATUS, 0x00};

    SPI_EEPROM_UNSELECT();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SPI_EEPROM_SELECT();

        spi_transfer(ubBuf, 2, ubBuf);

        SPI_EEPROM_UNSELECT();
    }

    return ubBuf[1];
}
