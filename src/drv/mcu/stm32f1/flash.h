// SPDX-License-Identifier: MIT

#ifndef __FLASH_H__
#define __FLASH_H__

#include "config.h"
#include <stm32f1xx.h>

//--------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------

#define FLASH_SIZE          (((uint32_t)*(volatile uint16_t *)0x1FFFF7E0) << 10)
#define FLASH_PAGE_SIZE     2048
#define FLASH_PAGE_COUNT    (FLASH_SIZE / FLASH_PAGE_SIZE)
#define FLASH_MAX_ADDRESS   (FLASH_BASE + FLASH_SIZE - 1)

//--------------------------------------------------------------------
// Funcion declarations
//--------------------------------------------------------------------

void flash_init();
void flash_latency_config(uint32_t ulSystemClock);
void flash_lock();
void flash_unlock();
void flash_page_erase(uint32_t ulAddress);
void flash_page_write(uint32_t ulAddress, uint8_t *pubData, uint32_t ulSize);

#endif
