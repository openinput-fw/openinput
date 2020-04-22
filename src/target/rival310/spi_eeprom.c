// SPDX-License-Identifier: MIT

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
