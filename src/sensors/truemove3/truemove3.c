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

#include "truemove3.h"

//--------------------------------------------------------------------
// Datatypes
//--------------------------------------------------------------------

typedef struct {
    uint8_t motion;
    uint8_t observation;
    int16_t deltaX;
    int16_t deltaY;
    uint8_t squal;
} motion_burst_t;

//--------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------

uint8_t motionFlag = 0;

volatile int32_t truemove3DeltaX = 0, truemove3DeltaY = 0;

//--------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------

uint8_t truemove3_spi_transfer(uint8_t byte) __attribute__ ((weak));
void truemove3_select() __attribute__ ((weak));
void truemove3_unselect() __attribute__ ((weak));
void truemove3_delay_ms(uint32_t ticks) __attribute__ ((weak));
void truemove3_delay_us(uint32_t ticks) __attribute__ ((weak));

//--------------------------------------------------------------------
// Funcions
//--------------------------------------------------------------------

uint8_t truemove3_read(uint8_t address)
{
    uint8_t value;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        truemove3_select();

        truemove3_spi_transfer(address & 0x7F); // 7 bit address + read bit(0)
        truemove3_delay_us(120); // Tsrad
        value = truemove3_spi_transfer(0x00);

        truemove3_unselect();
    }
    truemove3_delay_us(160);

    return value;
}
void truemove3_write(uint8_t address, uint8_t value)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        truemove3_select();

        truemove3_spi_transfer((address & 0x7F) | 0x80); // 7 bit address + write bit(1)
        truemove3_delay_us(120); // Tsrad
        truemove3_spi_transfer(value);

        truemove3_unselect();
    }
    truemove3_delay_us(160);
}

motion_burst_t truemove3_read_motion_burst()
{
    motion_burst_t motionBurst = {0};

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t buf[6];
        uint8_t* buf_ptr = buf;

        truemove3_select();

        truemove3_spi_transfer(TRUEMOVE3_REG_MOT_BURST); // 7 bit address + read bit(0)
        truemove3_delay_us(35); // Tsrad_motbr
        for(uint8_t i = 0; i < 6; i++)
            *buf_ptr++ = truemove3_spi_transfer(0x00);

        truemove3_unselect();

        motionBurst.motion = buf[0];
        motionBurst.observation = buf[1];
        motionBurst.deltaX |= buf[2];
        motionBurst.deltaX |= (buf[3] << 8);
        motionBurst.deltaY |= buf[4];
        motionBurst.deltaY |= (buf[5] << 8);
    }
    truemove3_delay_us(160);

    return motionBurst;
}

uint8_t truemove3_init(uint8_t* firmware)
{
    truemove3DeltaX = 0;
    truemove3DeltaY = 0;

    if(!firmware)
        return 0;

    truemove3_unselect();
    truemove3_delay_ms(1);

    // software reset
    truemove3_write(TRUEMOVE3_REG_PWR_UP_RST, TRUEMOVE3_RESET_CMD);
    truemove3_delay_ms(50);

    truemove3_read(TRUEMOVE3_REG_MOTION);
    truemove3_read(TRUEMOVE3_REG_DELTA_X_L);
    truemove3_read(TRUEMOVE3_REG_DELTA_X_H);
    truemove3_read(TRUEMOVE3_REG_DELTA_Y_L);
    truemove3_read(TRUEMOVE3_REG_DELTA_Y_H);

    if(truemove3_read(TRUEMOVE3_REG_PID) != 0x42) // confirm comm link
        return 0;

    truemove3_write(TRUEMOVE3_REG_CONFIG2, 0x00); // clear REST enable bit

    truemove3_write(TRUEMOVE3_REG_SROM_EN, TRUEMOVE3_SROM_DWNLD_CMD); // initialize SROM download
    truemove3_delay_ms(15);

    truemove3_write(TRUEMOVE3_REG_SROM_EN, TRUEMOVE3_SROM_DWNLD_START_CMD); // start SROM download

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        truemove3_select();

        truemove3_spi_transfer(TRUEMOVE3_REG_SROM_BURST | 0x80); // 7 bit srom_burst address + write bit(1)
        truemove3_delay_us(15);

        for(uint16_t b = 0; b < 4094; b++) // write firmware image (4094 bytes)
        {
            truemove3_spi_transfer(*firmware++);
            truemove3_delay_us(15);
        }

        truemove3_unselect();
    }
    truemove3_delay_ms(2); // Tbexit

    uint8_t sromId = truemove3_read(TRUEMOVE3_REG_SROM_ID); // read srom firmware id

    if(!sromId)
        return 0;

    truemove3_write(TRUEMOVE3_REG_SROM_EN, TRUEMOVE3_SROM_CRC_CMD); // initialize CRC check
    truemove3_delay_ms(15);

    volatile uint16_t sromCrc = 0;

    sromCrc |= (truemove3_read(TRUEMOVE3_REG_DOUT_H) << 8);
    sromCrc |= truemove3_read(TRUEMOVE3_REG_DOUT_L);

    if(!sromCrc) //TODO: check CRC
        return 0;

    if(!(truemove3_read(TRUEMOVE3_REG_OBSERVATION) & TRUEMOVE3_SROM_RUN)) // check SROM running bit
        return 0;

    truemove3_write(TRUEMOVE3_REG_CONFIG2, 0x00); // clear REST enable bit

    truemove3_write(TRUEMOVE3_REG_MOT_BURST, 0x01);

    truemove3_delay_ms(500);

    truemove3_read_motion_burst();

    truemove3_write(TRUEMOVE3_REG_OBSERVATION, 0x00); // clear observation register

    truemove3_write(TRUEMOVE3_REG_CONFIG1, TRUEMOVE3_CPI_(1400)); // set CPI (default = 0x31 = 5000cpi)

    truemove3_read_motion_burst();

    return sromId;
}

void truemove3_task()
{
    if(motionFlag)
    {
        motionFlag = 0;
        motion_burst_t burst = truemove3_read_motion_burst();
        truemove3DeltaX += burst.deltaX;
        truemove3DeltaY += burst.deltaY;
    }
}

void truemove3_motion_event()
{
    motionFlag = 1;
}

deltas_t truemove3_get_deltas()
{
    deltas_t deltas;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(truemove3DeltaX > __INT8_MAX__)
        {
            deltas.dx = __INT8_MAX__;
            truemove3DeltaX -= __INT8_MAX__;
        }
        else if(truemove3DeltaX < -__INT8_MAX__)
        {
            deltas.dx = -__INT8_MAX__;
            truemove3DeltaX += __INT8_MAX__;
        }
        else
        {
            deltas.dx = truemove3DeltaX;
            truemove3DeltaX = 0;
        }

        if(truemove3DeltaY > __INT8_MAX__)
        {
            deltas.dy = __INT8_MAX__;
            truemove3DeltaY -= __INT8_MAX__;
        }
        else if(truemove3DeltaY < -__INT8_MAX__)
        {
            deltas.dy = -__INT8_MAX__;
            truemove3DeltaY += __INT8_MAX__;
        }
        else
        {
            deltas.dy = truemove3DeltaY;
            truemove3DeltaY = 0;
        }
    }

    return deltas;
}