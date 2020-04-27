// SPDX-License-Identifier: MIT

#ifndef __TRUEMOVE3_H__
#define __TRUEMOVE3_H__

#include "config.h"
#include <stdlib.h>
#include "atomic.h"
#include "target.h"

//--------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------

// general registers
#define TRUEMOVE3_REG_PID           0x00
#define TRUEMOVE3_REG_PWR_UP_RST    0x3A

// motion registers
#define TRUEMOVE3_REG_MOTION        0x02
#define TRUEMOVE3_REG_DELTA_X_L     0x03
#define TRUEMOVE3_REG_DELTA_X_H     0x04
#define TRUEMOVE3_REG_DELTA_Y_L     0x05
#define TRUEMOVE3_REG_DELTA_Y_H     0x06
#define TRUEMOVE3_REG_MOT_BURST     0x50

// srom registers
#define TRUEMOVE3_REG_SROM_EN       0x13
#define TRUEMOVE3_REG_SROM_ID       0x2A
#define TRUEMOVE3_REG_SROM_BURST    0x62

// config registers
#define TRUEMOVE3_REG_CONFIG1       0x0F
#define TRUEMOVE3_REG_CONFIG2       0x10
#define TRUEMOVE3_REG_OBSERVATION   0x24
#define TRUEMOVE3_REG_DOUT_L        0x25
#define TRUEMOVE3_REG_DOUT_H        0x26

// motion bits
#define TRUEMOVE3_MOTION            0x80
#define TRUEMOVE3_OPMODE_RUN        0x00
#define TRUEMOVE3_OPMODE_REST1      0x02
#define TRUEMOVE3_OPMODE_REST2      0x04
#define TRUEMOVE3_OPMODE_REST3      0x06

// config2 bits
#define TRUEMOVE3_RESTEN            0x20
#define TRUEMOVE3_RPT_MOD           0x04       // XY CPI reporting mode (different cpis)

// observation bits
#define TRUEMOVE3_SROM_RUN          0x40

// srom cmds
#define TRUEMOVE3_SROM_CRC_CMD          0x15
#define TRUEMOVE3_SROM_DWNLD_CMD        0x1D
#define TRUEMOVE3_SROM_DWNLD_START_CMD  0x18

// power up reset cmd
#define TRUEMOVE3_RESET_CMD         0x5A

// cpis
#define TRUEMOVE3_CPI_(cpi)         ((cpi / 100) - 1)   // 100-12000, 100 cpi LSb
#define TRUEMOVE3_CPI_DEFAULT       0x31                // 5000 cpi
#define TRUEMOVE3_CPI_MAX           0x77                // 12000 cpi
#define TRUEMOVE3_CPI_MIN           0x00                // 100 cpi

// product unique identifier
#define TRUEMOVE3_PID   0x42

//--------------------------------------------------------------------
// Target Porting API
//--------------------------------------------------------------------

uint8_t truemove3_init(uint8_t* firmware);

void truemove3_task();

void truemove3_motion_event();

deltas_t truemove3_get_deltas();

#endif // __TRUEMOVE3_H__