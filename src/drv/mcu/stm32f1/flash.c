// SPDX-License-Identifier: MIT

#include "flash.h"

//--------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------

void flash_init()
{
    // HSI must be on during flash write/erase opeartions
    RCC->CR |= RCC_CR_HSION; // Enable HSI

    while(!(RCC->CR & RCC_CR_HSIRDY)); // Wait until HSI ready
}
void flash_latency_config(uint32_t ulSystemClock)
{
    FLASH->ACR &= ~FLASH_ACR_LATENCY;

    if(ulSystemClock <= 24000000)
    {
        FLASH->ACR |= 0; // 0 wait states
    }
    else if(ulSystemClock <= 48000000)
    {
        FLASH->ACR |= 1; // 1 wait state
    }
    else
    {
        FLASH->ACR |= 2; // 2 wait states
    }
}
void flash_lock()
{
    FLASH->CR |= FLASH_CR_LOCK;
}
void flash_unlock()
{
    FLASH->KEYR = 0x45670123; // KEY 1
    FLASH->KEYR = 0xCDEF89AB; // KEY 2
}
void flash_page_erase(uint32_t ulAddress)
{
    if(ulAddress < FLASH_BASE || ulAddress > FLASH_MAX_ADDRESS)
        return;

    if(FLASH->CR & FLASH_CR_LOCK)
        return;

    ulAddress &= ~(uint32_t)(FLASH_PAGE_SIZE - 1);

    while(FLASH->SR & FLASH_SR_BSY);

    FLASH->CR &= ~0x00000007; // Clear all operation flags
    FLASH->CR |= FLASH_CR_PER;

    FLASH->AR = ulAddress;

    FLASH->CR |= FLASH_CR_STRT;

    while(FLASH->SR & FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_CR_PER;
}
void flash_page_write(uint32_t ulAddress, uint8_t *pubData, uint32_t ulSize)
{
    if(ulAddress < FLASH_BASE || ulAddress > FLASH_MAX_ADDRESS)
        return;

    if(ulSize & 1) // Only allow even data sizes
        return;

    if(FLASH->CR & FLASH_CR_LOCK)
        return;

    ulAddress &= ~(uint32_t)(FLASH_PAGE_SIZE - 1);

    while(FLASH->SR & FLASH_SR_BSY);

    FLASH->CR &= ~0x00000007; // Clear all operation flags
    FLASH->CR |= FLASH_CR_PG;

    while(ulSize)
    {
        *(volatile uint16_t *)ulAddress = *(uint16_t *)pubData;

        while(FLASH->SR & FLASH_SR_BSY);

        ulAddress += 2;
        pubData += 2;
        ulSize -= 2;
    }

    FLASH->CR &= ~FLASH_CR_PG;
}