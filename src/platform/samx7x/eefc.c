/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/samx7x/atomic.h"
#include "platform/samx7x/eefc.h"

#define ROM_BASE_ADDR 0x00400000ul

#define EEFC_ERROR_FLAGS                                                                                                 \
	(EEFC_FSR_FLOCKE_Msk | EEFC_FSR_FCMDE_Msk | EEFC_FSR_FLERR_Msk | EEFC_FSR_UECCELSB_Msk | EEFC_FSR_MECCELSB_Msk | \
	 EEFC_FSR_UECCEMSB_Msk | EEFC_FSR_MECCEMSB_Msk)

static struct flash_info_t flash_info;

/*
 * Local function declaration.
 * Because they are RAM functions, they need 'extern' declaration.
 */
extern void eefc_write_fmr(u32 fmr);
extern u32 eefc_perform_fcr(u32 fcr);

static u32 eefc_get_result();
static u32 eefc_get_status();
static void eefc_perform_command(u32 command, u32 argument);

void eefc_init()
{
	/* enable code loop optimization and sequential code optimization */
	eefc_write_fmr(EFC->EEFC_FMR & ~(EEFC_FMR_FWS_Msk | EEFC_FMR_SCOD_Msk) | EEFC_FMR_CLOE_Msk | EEFC_FMR_FWS(6));

	eefc_perform_command(EEFC_FCR_FCMD_GETD, 0);
	while (!(eefc_get_status() & EEFC_FSR_FRDY_Msk)) continue;
	eefc_get_result(); /* dummy read: id */
	flash_info.size = eefc_get_result();
	flash_info.page_size = eefc_get_result();
	flash_info.base_addr = ROM_BASE_ADDR;
}

void eefc_set_waitstates(u32 frequency)
{
	if (frequency <= 23000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(0);
	else if (frequency <= 46000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(1);
	else if (frequency <= 69000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(2);
	else if (frequency <= 92000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(3);
	else if (frequency <= 115000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(4);
	else if (frequency <= 138000000UL)
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(5);
	else
		EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(6);
}

void eefc_tcm_disable()
{
	/* TCM Size Configuration */
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(8));
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(7));

	__DSB();
	__ISB();
	SCB->ITCMCR &= ~(u32) (1UL);
	SCB->DTCMCR &= ~(u32) SCB_DTCMCR_EN_Msk;
	__DSB();
	__ISB();
}

const struct flash_info_t *eefc_get_info()
{
	return (const struct flash_info_t *) &flash_info;
}

s32 eefc_addr_to_page(u32 addr)
{
	if (addr < flash_info.base_addr)
		return -1;

	/* align to pages */
	addr -= (flash_info.base_addr);
	addr -= (addr % flash_info.page_size);

	return (addr / flash_info.page_size);
}

u32 eefc_page_to_addr(u32 page)
{
	return ((page * flash_info.page_size) + flash_info.base_addr);
}

u32 eefc_erase_16(u32 addr)
{
	u32 page = eefc_addr_to_page(addr);

	/* give erase and write page command */
	eefc_perform_command(EEFC_FCR_FCMD_EPA, (page & ~0xF) | 2); /* 16 pages (minimum for large sectors) */
	while (!(eefc_get_status() & EEFC_FSR_FRDY_Msk)) continue;
}

void eefc_read(void *buffer, u32 addr, u32 size)
{
	u8 *src = (u8 *) addr;
	u8 *dst = (u8 *) buffer;
	while (size--) {
		*dst++ = *src++;
	}
}

void eefc_write(u32 addr, void *buffer, u32 size)
{
	/* require 128 bit alignment, and single page writes */
	if ((addr % 16) || (size % 16) || ((size + (addr % 16)) > flash_info.page_size)) {
		return;
	}

	size /= 4;
	u32 *dst = (u32 *) addr;
	u32 *src = (u32 *) buffer;
	while (size--) {
		*dst++ = *src++;
	}

	u32 page = eefc_addr_to_page(addr);

	/* give erase and write page command */
	eefc_perform_command(EEFC_FCR_FCMD_WP, page);
	while (!(eefc_get_status() & EEFC_FSR_FRDY_Msk)) continue;
}

static u32 eefc_get_status()
{
	return EFC->EEFC_FSR;
}

static u32 eefc_get_result()
{
	return EFC->EEFC_FRR;
}

static void eefc_perform_command(u32 command, u32 argument)
{
	/* Unique ID commands are not supported. */
	if (command == EEFC_FCR_FCMD_STUI || command == EEFC_FCR_FCMD_SPUI) {
		return;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		/* Use RAM Function. */
		eefc_perform_fcr(EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FARG(argument) | EEFC_FCR_FCMD(command));
	}
}

__attribute__((noinline, section(".ramfunc"))) void eefc_write_fmr(u32 fmr)
{
	EFC->EEFC_FMR = fmr;
}

__attribute__((noinline, section(".ramfunc"))) u32 eefc_perform_fcr(u32 fcr)
{
	volatile u32 status;

	EFC->EEFC_FCR = fcr;
	do {
		status = EFC->EEFC_FSR;
	} while (!(status & EEFC_FSR_FRDY_Msk));

	return (status & EEFC_ERROR_FLAGS);
}
